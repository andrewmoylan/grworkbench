#pragma once

#include <boost/optional.hpp>
#include "lax.hpp"

namespace lax_interpreter
{
  namespace values
  {
    class definition : public value
    {
    public:
      virtual ~definition();
      
      static const shared_ptr<const value> make(const string&);

      virtual const lax evaluated() const;
      virtual const lax evaluated_at(const lax&) const;
      
      virtual void output(ostream&) const;
      
      virtual const bool is_boolean() const;
      virtual const bool is_character() const;
      virtual const bool is_integer() const;
      virtual const bool is_real() const;
      
      virtual const bool get_boolean() const;
      virtual const char get_character() const;
      virtual const int get_integer() const;
      virtual const double get_real() const;

      void assign(const lax&) const;
			void unassign() const;

			virtual const lax get_left() const;
      virtual const lax get_right() const;

    protected:
			explicit definition(const string&);

      const string _name;
			mutable optional<lax> _;

    private:
      definition();
      definition(const definition&);
      definition& operator=(const definition&);

			const lax validated() const;
    };

		class expanding_definition : public definition
		{
		public:
			virtual ~expanding_definition();

			static const shared_ptr<const value> make(const string&);

			virtual const lax evaluated_at(const lax&) const;
			virtual const lax bound_to(const lax&) const;
      virtual void output(ostream& o) const;

		private:
			explicit expanding_definition(const string&);
			mutable bool recursing;
		};
  }
}
