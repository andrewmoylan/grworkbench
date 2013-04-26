#pragma once

#include "lax/lax.hpp"
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <map>

namespace grwb
{
  namespace lax_integration
  {
    using lax_interpreter::lax;
    using namespace boost;
    using lax_interpreter::values::value;

    template <class T> class typed_lax_interface
    {
    public:
      typedef typename T::result_type return_type;
      typedef typename T::argument_type argument_type;

      static const shared_ptr<T> make(const lax& l)
      {
        static map<shared_ptr<const value>, shared_ptr<T> > _;
        const typename map<shared_ptr<const value>, shared_ptr<T> >::const_iterator search(_.find(l.value()));
        return search != _.end() ? search->second : _.insert(make_pair(l.value(), shared_ptr<T>(new T(typed_lax_interface(l))))).first->second;
      }

      return_type operator()(argument_type a) const
      {
        try
        {
          return _(a);
        }
        catch (...)
        {
          //throw "Error calling lax-defined function in C++";
					throw;
        }
      }

    protected:
      typed_lax_interface(const lax& l)
        : _(l)
      {}

    private:
      const lax _;
      typed_lax_interface& operator=(const typed_lax_interface&);
    };
  }
}
