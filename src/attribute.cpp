// ensure CppConcat is defined
#include <postgresql/c.h>
#include <postgresql/catalog/pg_type.h>
#include <active_record/attribute.h>
#include <active_record/exception.h>

namespace ActiveRecord {

Type::Type Attribute::type() const {
  int type_index = which();
  return index_to_type( type_index );
}

bool Attribute::operator==( const Attribute& other ) const {
  if( which() != other.which() )
    return false;
  switch( which() ) {
  case 0:
    return boost::get< int >( *this ) == boost::get< int >( other );
  case 1:
    return boost::get< long long >( *this ) == boost::get< long long >( other );
  case 2:
    return boost::get< string >( *this ) == boost::get< string >( other );
  case 3:
    return boost::get< double >( *this ) == boost::get< double >( other );
  case 4:
    return boost::get< Date >( *this ) == boost::get< Date >( other );
  default:
    throw ActiveRecordException( "Unexpected Attribute type", __FILE__, __LINE__ );
  }
}

Attribute Attribute::from_field(sqlite3_stmt *pStmt, int i) {
  const char * type = sqlite3_column_decltype(pStmt, i);
  if(type == NULL) {
    // http://www.sqlite.org/capi3ref.html#sqlite3_column_decltype
    // This is either an actual NULL value or SQLite doesn't know the
    // declared type of the column.
    const char * value = (const char *) sqlite3_column_text(pStmt, i);
    if(value != 0)
      return value;
    else
      return Attribute();
  } else if(strcasecmp( type, "INTEGER") == 0) {
    return sqlite3_column_int( pStmt, i );
  } else if(strcasecmp( type, "FLOAT") == 0) {
    return sqlite3_column_double( pStmt, i);
  } else if(strcasecmp( type, "TEXT") == 0) {
    const char * value = (const char *) sqlite3_column_text(pStmt, i);
    if(value != 0)
      return value;
    else
      return Attribute(); // TEXT hasn't been set
  } else if(strcasecmp(type, "DATE") == 0) {
    const char * value = (const char *) sqlite3_column_text(pStmt, i);
    if(value != 0)
      return Date::parse(value);
    else
      return Attribute(); // DATE hasn't been set
  } else {
    stringstream error;
    error << "Unhandled data type: " << type;
    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }
}

Attribute Attribute::from_field(PGresult * exec_result, int i) {
  Oid type   = PQftype(exec_result, i);
  char * raw = PQgetvalue(exec_result, 0, i);
  switch(type) {
    case CHAROID:
      return raw;
    case INT2OID:
    case INT4OID:
      return atoi(raw);
    case INT8OID:
      return atoll(raw);
    default: {
      stringstream error;
      error << "Value '" << raw << "' has unhandled data type " << type;
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
  }
}

ostream & operator<<( ostream &cout, ActiveRecord::Date const &date ) {
  cout << date.to_string();
  return cout;
}

} // namespace ActiveRecord
