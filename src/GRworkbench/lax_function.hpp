#pragma once

#include <iostream>
#include "lax_storage.hpp"
#include "atlas.hpp"
//#include "to_from_lax.hpp"

namespace grwb
{
  namespace lax_integration
  {
    namespace values
    {
      using namespace std;
      using namespace boost;

      template <class T> class lax_function : public storage<T>
      {
      public:
        virtual ~lax_function()
        {}

        virtual const lax evaluated_at(const lax& a) const
        {
          return (*(this->item()))(a);
        }

        static const lax make(const T& c, const string& name)
        {
          typedef pair<T, string> key_type;
          key_type t(make_pair(c, name));
          static map<key_type, lax> _;
          const typename map<key_type, lax>::const_iterator i(_.find(t));
          return i != _.end() ? i->second : _.insert(make_pair(t, lax(shared_ptr<const value>(new lax_function(c, name))))).first->second;
        }

      protected:
        lax_function(const T& c, const string& name)
            : storage<T>(c, name)
        {}
      private:
        lax_function& operator=(const lax_function&);
        lax_function(const lax_function&);
      };
    }
  }
}
