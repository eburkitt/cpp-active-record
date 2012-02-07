#include "test_helper.h"
#include <active_record/connection/postgresql.h>

/* N.B. These tests assume that a 'template1' database exists
 *      and that the supplied user can access that database
 *      and can create new databases */

/*
class PostgresqlConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ActiveRecord::set_debug( false );
    postgresql_shell_drop_database( "active_record_postgresql_test", Q(PG_USER) );
  }
};

TEST_F( PostgresqlConnectionTest, ConnectToExistentDatabase ) {
  PostgresqlConnection connection;
  ASSERT_NO_THROW(
    connection.connect( options
                        ( "database", "template1" )
                        ( "username", Q(PG_USER) ) ) );
}

TEST_F( PostgresqlConnectionTest, ConnectToNonExistentDatabase ) {
  PostgresqlConnection connection;
  ASSERT_THROW(
    connection.connect( options
                        ( "database", "database_that_does_not_exist" )
                        ( "username", Q(PG_USER) ) ),
    ActiveRecord::ActiveRecordException );
}

TEST_F( PostgresqlConnectionTest, DatabaseExists ) {
  PostgresqlConnection connection;
  connection.connect( options
                      ( "database", "template1" )
                      ( "username", Q(PG_USER) ) );
  bool exists = PostgresqlConnection::database_exists( connection, "template1" );
  ASSERT_TRUE( exists );
}

TEST_F( PostgresqlConnectionTest, CreateDatabase ) {
  PostgresqlConnection connection;
  connection.connect( options
                      ( "database", "template1" )
                      ( "username", Q(PG_USER) ) );
  bool created = PostgresqlConnection::create( connection, options
                                                           ( "database", "active_record_postgresql_test" )
                                                           ( "owner",    Q(PG_USER) ) );
  ASSERT_TRUE( created );
}

TEST_F( PostgresqlConnectionTest, DropDatabase ) {
  postgresql_shell_create_database( "active_record_postgresql_test", "template1", Q(PG_USER) );
  ASSERT_TRUE( postgresql_shell_database_exists( "active_record_postgresql_test", Q(PG_USER) ) );

  PostgresqlConnection connection;
  connection.connect( options
                      ( "database", "template1" )
                      ( "username", Q(PG_USER) ) );
  ASSERT_NO_THROW(
    PostgresqlConnection::drop( connection, "active_record_postgresql_test" )
  );
  ASSERT_FALSE( postgresql_shell_database_exists( "active_record_postgresql_test", Q(PG_USER) ) );
}
*/

class PostgresqlWithConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    postgresql_shell_create_database( "active_record_postgresql_test", "template1", Q(PG_USER) );
    connection.connect( options
                        ( "database", "active_record_postgresql_test" )
                        ( "username", Q(PG_USER) ) );
   }
  virtual void TearDown() {
    connection.disconnect();
    postgresql_shell_drop_database( "active_record_postgresql_test", Q(PG_USER) );
  }
 protected:
  PostgresqlConnection connection;
};

TEST_F( PostgresqlWithConnectionTest, TableExists ) {
  connection.execute( "CREATE TABLE foo ()" );

  ASSERT_TRUE( connection.table_exists( "foo" ) );
  ASSERT_FALSE( connection.table_exists( "bar" ) );
} 

