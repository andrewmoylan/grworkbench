#include <iostream>
#include <map>
#include "basic_types.hpp"

namespace lax_interpreter
{
	namespace values
	{
		boolean::boolean(const bool b)
			:	_(b)
		{
		}

		boolean::~boolean()
		{
		}

		const shared_ptr<const boolean> boolean::make(const bool b)
		{
			if (b)
			{
				static const shared_ptr<const boolean> _(new boolean(true));
				return _;
			}
			else
			{
				static const shared_ptr<const boolean> _(new boolean(false));
				return _;
			}
		}

		void boolean::output(ostream& o) const
		{
			o << (_ ? "true" : "false");
		}

		const bool boolean::is_boolean() const
		{
			return true;
		}

		const bool boolean::get_boolean() const
		{
			return _;
		}

		character::character(const char c)
			: _(c)
		{
		}

		character::~character()
		{
		}

		const shared_ptr<const character> character::make(const char c)
		{
			static map<char, shared_ptr<const character> > _;
			const map<char, shared_ptr<const character> >::const_iterator i(_.find(c));
			return i != _.end() ? i->second : _.insert(make_pair(c, shared_ptr<const character>(new character(c)))).first->second;
		}

		void character::output(ostream& o) const
		{
			o << '\'' << _ << '\'';
		}

		const bool character::is_character() const
		{
			return true;
		}

		const char character::get_character() const
		{
			return _;
		}

		integer::integer(const int i)
			:	_(i)
		{
		}

		integer::~integer()
		{
		}

		const shared_ptr<const value> integer::make(const int i)
		{
			static map<int, shared_ptr<const value> > _;
			const map<int, shared_ptr<const value> >::const_iterator j(_.find(i));
			return j != _.end() ? j->second : _.insert(make_pair(i, shared_ptr<const value>(new integer(i)))).first->second;
		}

		void integer::output(ostream& o) const
		{
			o << _;
		}

		const bool integer::is_integer() const
		{
			return true;
		}

		const int integer::get_integer() const
		{
			return _;
		}

		const bool integer::is_real() const
		{
			return true;
		}

		const double integer::get_real() const
		{
			return _;
		}

		real::real(const double d)
			:	_(d)
		{
		}

		real::~real()
		{
		}

		const shared_ptr<const value> real::make(const double& d)
		{
			if (d == int(d))
				return integer::make(int(d));

			static map<double, shared_ptr<const value> > _;
			const map<double, shared_ptr<const value> >::const_iterator i(_.find(d));
			return i != _.end() ? i->second : _.insert(make_pair(d, shared_ptr<const value>(new real(d)))).first->second;
		}

		void real::output(ostream& o) const
		{
			o << _;
		}

		const bool real::is_real() const
		{
			return true;
		}

		const double real::get_real() const
		{
			return _;
		}
	}
}
