#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace lax_interpreter
{
	class lax;

	namespace values
	{
		using namespace std;
		using namespace boost;

		class value : public enable_shared_from_this<value>
		{
		public:
			virtual ~value();

			virtual const lax evaluated() const;
			virtual const lax evaluated_at(const lax&) const;
			virtual const lax bound_to(const lax& r) const;

			virtual const bool is_call() const;
			virtual void output(ostream&) const = 0;

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

		protected:
			value();

		private:
			value(const value&);
			value& operator=(const value&);
		};
	}
}

