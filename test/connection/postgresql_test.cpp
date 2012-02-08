#include "test_helper.h"
#include <active_record/connection/postgresql.h>

/* N.B. These tests assume that a 'template1' database exists
 *      and that the supplied user can access that database
 *      and can create new databases */

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

class PostgresqlWithConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connection.connect( options
                        ( "database", "template1" )
                        ( "username", Q(PG_USER) ) );
   }
  virtual void TearDown() {
    connection.disconnect();
  }
 protected:
  PostgresqlConnection connection;
};

TEST_F( PostgresqlWithConnectionTest, CreateDatabase ) {
  bool created = PostgresqlConnection::create_database( connection, options
                                                                    ( "database", "active_record_postgresql_test" )
                                                                    ( "owner",    Q(PG_USER) ) );
  ASSERT_TRUE( created );
}

TEST_F( PostgresqlWithConnectionTest, DropDatabase ) {
  postgresql_shell_create_database( "active_record_postgresql_test", "template1", Q(PG_USER) );
  ASSERT_TRUE( postgresql_shell_database_exists( "active_record_postgresql_test", Q(PG_USER) ) );

  ASSERT_NO_THROW(
    PostgresqlConnection::drop_database( connection, "active_record_postgresql_test" )
  );
  ASSERT_FALSE( postgresql_shell_database_exists( "active_record_postgresql_test", Q(PG_USER) ) );
}

TEST_F( PostgresqlWithConnectionTest, DatabaseExists ) {
  ASSERT_TRUE( PostgresqlConnection::database_exists( connection, "template1" ) );
  ASSERT_FALSE( PostgresqlConnection::database_exists( connection, "database_that_does_not_exist" ) );
}

TEST_F( PostgresqlWithConnectionTest, TableExists ) {
  connection.execute( "CREATE TABLE foo ()" ); // TODO: use command line

  ASSERT_TRUE( connection.table_exists( "foo" ) );
  ASSERT_FALSE( connection.table_exists( "bar" ) );
} 

