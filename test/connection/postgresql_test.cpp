#include "test_helper.h"
#include <active_record/connection/postgresql.h>

/* N.B. These tests assume that a 'template1' database exists
 *      and that the supplied user can access that database
 *      and can create new databases */

class PostgresqlTest  : public ::testing::Test {
 protected:
  void connect(const string &name) {
    connection.connect(options
                       ("database", name.c_str())
                       ("username", Q(PG_USER)));
    connected_database = name;
  }
  void create_connect(const string &name) {
    postgresql_shell_drop_database(name, "template1", Q(PG_USER));
    postgresql_shell_create_database(name, "template1", Q(PG_USER));
    created_database = name;
    connect(created_database);
  }
  virtual void TearDown() {
    connection.disconnect();
    if(!created_database.empty())
      postgresql_shell_drop_database(created_database, "template1", Q(PG_USER));
  }
 protected:
  PostgresqlConnection connection;
  string               connected_database;
  string               created_database;
};

class PostgresqlConnectionTest : public ::testing::Test {
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

class PostgresqlDatabasesTest : public PostgresqlTest {
 protected:
  virtual void SetUp() {
    postgresql_shell_drop_database("active_record_test_database", "template1", Q(PG_USER));
  }
  virtual void TearDown() {
    PostgresqlTest::TearDown();
    postgresql_shell_drop_database("active_record_test_database", "template1", Q(PG_USER));
  }
};

TEST_F(PostgresqlDatabasesTest, CreateDatabase ) {
  connect("template1");
  bool created = PostgresqlConnection::create_database( connection, options
                                                                    ( "database", "active_record_test_database" )
                                                                    ( "owner",    Q(PG_USER) ) );
  ASSERT_TRUE(created);
  ASSERT_TRUE(postgresql_shell_database_exists("active_record_test_database", Q(PG_USER)));
}

TEST_F(PostgresqlDatabasesTest, DropDatabase ) {
  postgresql_shell_create_database( "active_record_test_database", "template1", Q(PG_USER) );
  ASSERT_TRUE( postgresql_shell_database_exists( "active_record_test_database", Q(PG_USER) ) );
  connect("template1");

  ASSERT_NO_THROW(
    PostgresqlConnection::drop_database( connection, "active_record_test_database" )
  );
  ASSERT_FALSE( postgresql_shell_database_exists( "active_record_test_database", Q(PG_USER) ) );
}

TEST_F(PostgresqlDatabasesTest, DatabaseExists ) {
  connect("template1");
  ASSERT_TRUE( PostgresqlConnection::database_exists( connection, "template1" ) );
  ASSERT_FALSE( PostgresqlConnection::database_exists( connection, "database_that_does_not_exist" ) );
}

class PostgresqlTablesTest : public PostgresqlTest {
 protected:
  virtual void SetUp() {
    create_connect("active_record_test_database");
  }
};

TEST_F(PostgresqlTablesTest, TableExists) {
  postgresql_shell_command(created_database, Q(PG_USER), "CREATE TABLE foo ()");

  ASSERT_TRUE(connection.table_exists( "foo" ));
  ASSERT_FALSE(connection.table_exists( "bar" ));
} 

class PostgresqlQueriesTest : public PostgresqlTest {
 protected:
  virtual void SetUp() {
    create_connect("active_record_test_database");
    postgresql_shell_command(created_database, Q(PG_USER), "CREATE TABLE foo (id SERIAL, bar INTEGER)");
  }
};

TEST_F(PostgresqlQueriesTest, Execute) {
  connection.execute("CREATE TABLE bar ()");

  ASSERT_TRUE(connection.table_exists("bar"));
} 

TEST_F(PostgresqlQueriesTest, Insert) {
  long id = connection.insert("INSERT INTO foo (bar) VALUES (1);");

  ASSERT_EQ(id, 1);
  list<string> output = postgresql_shell_command(connected_database, Q(PG_USER), "SELECT bar FROM foo");
  ASSERT_EQ(" bar \n", output.front());
  output.pop_front();
  output.pop_front();
  ASSERT_EQ("   1\n", output.front());
} 

TEST_F(PostgresqlQueriesTest, SelectValue) {
  connection.execute("INSERT INTO foo (bar) VALUES (42);");

  ActiveRecord::Attribute result = connection.select_value("SELECT bar FROM foo");
  ASSERT_TRUE(result.has_data());
  ASSERT_EQ(result.type(), ActiveRecord::integer);
  ASSERT_EQ(boost::get<int>(result), 42);
} 

