#pragma once

#include "lax/lax.hpp"
#include "lax/value.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>
#include <string>

namespace grwb
{
  namespace lax_integration
  {
    namespace values
    {
      using lax_interpreter::lax;
      using namespace boost;
      using lax_interpreter::values::value;
      using namespace std;

      template <class T> class storage : public value
      {
      public:
        virtual ~storage()
        {}

        static const lax make(const T& t, const string& name)
        {
          static map<T, lax> _;
          const typename map<T, lax>::const_iterator i(_.find(t));
          return i != _.end() ? i->second : _.insert(make_pair(t, lax(shared_ptr<const value>(new storage(t, name))))).first->second;
        }

        const T& item() const
        {
          return _;
        }

        virtual void output(std::ostream& o) const
        {
          o << name_;
        }

        static const optional<T> extract_from(const lax& l)
        {
          const shared_ptr<const storage<T> > s(l.dynamic_value<const storage<T> >());
          if (s)
            return s->item();
          else
            return optional<T>();

          //throw "Invalid extraction from storage";
        }

      protected:
        explicit storage(const T& t, const string& name)
            :  _(t),
            name_(name)
      {}

      private:
        const string name_;
        const T _;
      };
    }
  }
}
