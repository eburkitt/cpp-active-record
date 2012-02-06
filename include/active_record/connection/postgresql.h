#ifndef _ACTIVE_RECORD_CONNECTION_POSTGRESQL_H_
#define _ACTIVE_RECORD_CONNECTION_POSTGRESQL_H_

#include <libpq-events.h>
#include <active_record/connection.h>

namespace ActiveRecord {

class PostgresqlConnection : public Connection {
 public:
  PostgresqlConnection();
  virtual ~PostgresqlConnection();

  virtual void  connect( OptionsHash options );
  virtual void  disconnect();
  virtual bool  connected();

  // Database Structure
  virtual bool  table_exists( const string &table_name );
  // Queries
  virtual bool          execute( const string &query,
                                 const AttributeList &parameters = AttributeList() );
  virtual long          insert( const string &query,
                                const AttributeList &parameters = AttributeList() );
  //virtual Attribute     select_value( const string &query,
  //                                    const AttributeList &parameters = AttributeList() );
  //virtual AttributeList select_values( const string &query,
  //                                     const AttributeList &parameters = AttributeList() );
  virtual Row           select_one( const string &query,
                                    const AttributeList &parameters = AttributeList() );
  virtual RowSet        select_all( const string &query,
                                    const AttributeList &parameters = AttributeList() );
 private:
  PostgresqlConnection( const PostgresqlConnection& other );
  PostgresqlConnection operator=( const PostgresqlConnection& other );
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_CONNECTION_POSTGRESQL_H_

