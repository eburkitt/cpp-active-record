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

} // namespace ActiveRecord

