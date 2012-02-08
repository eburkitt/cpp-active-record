#include <active_record/connection/postgresql.h>

namespace ActiveRecord {

PostgresqlConnection::PostgresqlConnection() : pgconn_( NULL ) {}

PostgresqlConnection::~PostgresqlConnection() {
  disconnect();
}

PostgresqlConnection::PostgresqlConnection( const PostgresqlConnection& other ) {}

PostgresqlConnection PostgresqlConnection::operator=( const PostgresqlConnection& other ) {
  return *this;
}

bool PostgresqlConnection::create_database( PostgresqlConnection & connection, OptionsHash options ) {
  stringstream create_stream;
  create_stream << "CREATE DATABASE " << options[ "database" ] << " ";
  if( options.find( "owner" ) != options.end() )
    create_stream << "WITH OWNER " << options[ "owner" ] << " ";
  if( options.find( "template" ) != options.end() )
    create_stream << "TEMPLATE " << options[ "template" ] << " ";
  if( options.find( "encoding" ) != options.end() )
    create_stream << "ENCODING '" << options[ "encoding" ] << "' ";
  create_stream << ";";
  return connection.execute( create_stream.str() );
}

void PostgresqlConnection::drop_database( PostgresqlConnection & connection, const string &database_name ) {
  stringstream query;
  query << "DROP DATABASE " << database_name << ";";
  connection.execute( query.str() );
 }

bool PostgresqlConnection::database_exists( PostgresqlConnection & connection,
                                            const string &database_name ) {
  stringstream query;
  query << "SELECT datname FROM pg_database WHERE datname = '" << database_name << "';";
  Row row = connection.select_one( query.str() );
  return row.has_data();
}

/////////////////////////////////////////////////////
// Connection

void PostgresqlConnection::connect( OptionsHash options ) {
  stringstream connection_stream;
  connection_stream << "dbname=" << options[ "database" ] << " ";
  if( options.find( "host" ) != options.end() )
    connection_stream << "host=" << options[ "host" ] << " ";
  if( options.find( "username" ) != options.end() )
    connection_stream << "user=" << options[ "username" ] << " ";
  if( options.find( "port" ) != options.end() )
    connection_stream << "port=" << options[ "port" ] << " ";
  string conninfo = connection_stream.str(); 

  pgconn_ = PQconnectdb( conninfo.c_str() );

  ConnStatusType status = PQstatus( pgconn_ );
  if( status == CONNECTION_BAD ) {
    throw ActiveRecordException( "Connection to database failed", __FILE__, __LINE__ );
  }
}

void PostgresqlConnection::disconnect() {
  if( pgconn_ != NULL) {
    PQfinish( pgconn_ );
    pgconn_ = NULL;
  }
}

bool PostgresqlConnection::connected() {
  return pgconn_ != NULL ? true : false;
}

bool PostgresqlConnection::table_exists( const string &table_name ) {
  string query = \
    "SELECT c.relname as table       \
     FROM pg_catalog.pg_class c      \
     WHERE c.relname = '" + table_name + "'";
  Row row = select_one( query );
  return row.has_data();
}

bool PostgresqlConnection::execute( const string &query,
                                    const AttributeList &parameters ) {
  log( "PostgresqlConnection::execute" );
  log( query );
  PGresult * exec_result = PQexec( pgconn_, query.c_str() );
  bool success = is_error( exec_result );
  if( !success )
    log_error( exec_result );
  PQclear( exec_result );
  return success;
}

long PostgresqlConnection::insert( const string &query,
                         const AttributeList &parameters ) {
  log( "PostgresqlConnection::insert" );
  log( query );
  return 0;
}

Row PostgresqlConnection::select_one( const string &query,
                                      const AttributeList &parameters ) {
  log( "PostgresqlConnection::select_one" );
  log( query );
  PGresult * exec_result = PQexec( pgconn_, query.c_str() );
  bool success = is_error( exec_result );
  if( !success ) {
    log_error( exec_result );
    PQclear( exec_result );
    return Row();
  }
  int row_count = PQntuples( exec_result );
  if( row_count == 0 ) {
    log( "No data" );
    PQclear( exec_result );
    return Row();
  }
  string m = "row_count: " + row_count;
  log( m );
  Row row( exec_result, 0 );
  PQclear( exec_result );
  return row;
}

RowSet PostgresqlConnection::select_all( const string &query,
                                         const AttributeList &parameters ) {
  RowSet results;
  return results;
}

bool PostgresqlConnection::is_error( PGresult *exec_result ) {
  ExecStatusType status = PQresultStatus( exec_result );
  return ( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK ) ? false : true;
 }

void PostgresqlConnection::log_error( PGresult *exec_result ) {
  stringstream error;
  error << "PQexec returned an error. ";
  ExecStatusType status = PQresultStatus( exec_result );
  error << "Code: " << status << " ";
  error << "Message: " << PQresultErrorMessage( exec_result ) << " ";
  error << "" << PQresultErrorField( exec_result, PG_DIAG_MESSAGE_PRIMARY ) << " ";
  error << "" << PQresultErrorField( exec_result, PG_DIAG_MESSAGE_DETAIL ) << " ";
  error << "" << PQresultErrorField( exec_result, PG_DIAG_MESSAGE_HINT ) << " ";
  log( error.str() );
}

} // namespace ActiveRecord

