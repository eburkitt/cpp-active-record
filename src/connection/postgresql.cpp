#include <active_record/connection/postgresql.h>

namespace ActiveRecord {

PostgresqlConnection::PostgresqlConnection() : db_( NULL ) {}

PostgresqlConnection::~PostgresqlConnection() {
  disconnect();
}

PostgresqlConnection::PostgresqlConnection( const PostgresqlConnection& other ) {}

PostgresqlConnection PostgresqlConnection::operator=( const PostgresqlConnection& other ) {
  return *this;
}

