#pragma once

#include <boost/optional.hpp>
#include "value.hpp"
#include "lax.hpp"

namespace lax_interpreter 
{  
  namespace values
  { 
    class call : public value
    {
    public:
      virtual ~call();

      static const lax make(const lax&, const lax&);

      virtual const lax evaluated() const;
      virtual const lax evaluated_at(const lax&) const;

			virtual const bool is_call() const;

      virtual void output(ostream&) const;

      virtual const bool is_boolean() const;
      virtual const bool is_character() const;
      virtual const bool is_integer() const;
      virtual const bool is_real() const;

      virtual const bool get_boolean() const;
      virtual const char get_character() const;
      virtual const int get_integer() const;
      virtual const double get_real() const;

      virtual const lax get_left() const;
      virtual const lax get_right() const;

			void invalidate_cache() const;

			static void invalidate_caches();

    private:
      call();
      call(const call&);
      call& operator=(const call&);

			call(const lax&, const lax&);

			const lax cached_result() const;

      const lax _left;
      const lax _right;

      mutable optional<const lax> _cache;

			static map<pair<lax, lax>, shared_ptr<const value> > calls;
    };
  }
}
