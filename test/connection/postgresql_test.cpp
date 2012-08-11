#include "test_helper.h"
#include <active_record/connection/postgresql.h>

/* N.B. These tests assume that a 'template1' database exists
 *      and that the supplied user can access that database
 *      and can create new databases */

class PostgresqlConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ActiveRecord::set_debug( false );
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
    postgresql_shell_drop_database( "active_record_test_connection_database", "template1", Q(PG_USER) );
    postgresql_shell_drop_database( "active_record_test_create_database", "template1", Q(PG_USER) );
    postgresql_shell_create_database( "active_record_test_connection_database", "template1", Q(PG_USER) );
    connection.connect( options
                        ( "database", "active_record_test_connection_database" )
                        ( "username", Q(PG_USER) ) );
   }
  virtual void TearDown() {
    connection.disconnect();
    postgresql_shell_drop_database( "active_record_test_create_database", "template1", Q(PG_USER) );
    postgresql_shell_drop_database( "active_record_test_connection_database", "template1", Q(PG_USER) );
  }
 protected:
  PostgresqlConnection connection;
};

TEST_F( PostgresqlWithConnectionTest, CreateDatabase ) {
  bool created = PostgresqlConnection::create_database( connection, options
                                                                    ( "database", "active_record_test_create_database" )
                                                                    ( "owner",    Q(PG_USER) ) );
  ASSERT_TRUE(created);
  ASSERT_TRUE(postgresql_shell_database_exists("active_record_test_create_database", Q(PG_USER)));
}

TEST_F( PostgresqlWithConnectionTest, DropDatabase ) {
  postgresql_shell_create_database( "active_record_test_drop_database", "template1", Q(PG_USER) );
  ASSERT_TRUE( postgresql_shell_database_exists( "active_record_test_drop_database", Q(PG_USER) ) );

  ASSERT_NO_THROW(
    PostgresqlConnection::drop_database( connection, "active_record_test_drop_database" )
  );
  ASSERT_FALSE( postgresql_shell_database_exists( "active_record_test_drop_database", Q(PG_USER) ) );
}

TEST_F( PostgresqlWithConnectionTest, DatabaseExists ) {
  ASSERT_TRUE( PostgresqlConnection::database_exists( connection, "template1" ) );
  ASSERT_FALSE( PostgresqlConnection::database_exists( connection, "database_that_does_not_exist" ) );
}

TEST_F( PostgresqlWithConnectionTest, TableExists ) {
  postgresql_shell_command( "active_record_test_connection_database", Q(PG_USER), "CREATE TABLE foo ()" );

  ASSERT_TRUE( connection.table_exists( "foo" ) );
  ASSERT_FALSE( connection.table_exists( "bar" ) );
} 

