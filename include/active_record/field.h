#ifndef _ACTIVE_RECORD_FIELD_H_
#define _ACTIVE_RECORD_FIELD_H_

#include <active_record/type.h>

namespace ActiveRecord {

class Field {
 public:
  Field( const string &name, ActiveRecord::Type type ) : name_( name ), type_( type ) {}
  inline const string& name() const { return name_; }
  inline ActiveRecord::Type type() const { return type_; }
 private:
  string name_;
  ActiveRecord::Type type_;
};

class Fields : public vector< Field > {
 public:
  Fields operator-( Fields &other ) {
    Fields fields;
    for( Fields::iterator it = this->begin(); it != this->end(); ++it ) {
      if( ! other.has_field( it->name() ) )
        fields.push_back( Field( it->name(), it->type() ) );
    }
    return fields;
  }
  bool has_field( const string &field_name ) {
    for( Fields::iterator it = this->begin(); it != this->end(); ++it ) {
      if( it->name() == field_name ) {
        return true;
      }
    }
    return false;
  }
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_FIELD_H_
