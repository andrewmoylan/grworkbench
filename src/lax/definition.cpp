#include <iostream>
#include "definition.hpp"

namespace lax_interpreter
{
  namespace values
  {    
    const bool definition::is_boolean() const
    {
      return is_lax<bool>(validated());
    }
    
    const bool definition::is_character() const
    {
      return is_lax<char>(validated());
    }
    
    const bool definition::is_integer() const
    {
      return is_lax<int>(validated());
    }
    
    const bool definition::is_real() const
    {
      return is_lax<double>(validated());
    }
    
    const bool definition::get_boolean() const
    {
      return validated();
    }
    
    const char definition::get_character() const
    {
      return validated();
    }
    
    const int definition::get_integer() const
    {
      return validated();
    }
    
    const double definition::get_real() const
    {
      return validated();
    }

    void definition::assign(const lax& x) const
    {
      if (_)
				throw "Invalid call to void definition::assign(const lax&) const: definition was already assigned.";

			_ = optional<lax>(x);
    }

		void definition::unassign() const
		{
			_ = optional<lax>();
		}

    const lax definition::validated() const
    {
      if (!_)
				throw string("Invalid call to const lax& definition::validated() const: definition ") + _name + " was not yet assigned.";

			return *_;
    }

		const lax definition::get_left() const
		{
			throw "Invalid call to const lax definition::get_left() const";
		}

		const lax definition::get_right() const
		{
			throw "Invalid call to const lax definition::get_right() const";
		}

		definition::definition(const string& s)
			: _(),
			_name(s)
    {
    }
    
    definition::~definition()
    {
    }

    const shared_ptr<const value> definition::make(const string& s)
    {
      return shared_ptr<const value>(new definition(s));
    }

		void definition::output(ostream& o) const
    {
			o << _name;
    }

		const lax definition::evaluated_at(const lax& a) const
		{
			return validated().evaluated_at(a);
		}

		const lax definition::evaluated() const
		{
			return validated().evaluated();
		}

		expanding_definition::expanding_definition(const string& s)
			: definition(s),
			recursing(false)
    {
    }
    
    expanding_definition::~expanding_definition()
    {
    }

    const shared_ptr<const value> expanding_definition::make(const string& s)
    {
      return shared_ptr<const value>(new expanding_definition(s));
    }

		const lax expanding_definition::evaluated_at(const lax& a) const
		{
			if (recursing)
				return definition::bound_to(a);
			else
			{
				recursing = true;
				try
				{
					const lax b(definition::evaluated_at(a));
					recursing = false;
					return b;
				}
				catch (...)
				{
					recursing = false;
					throw;
				}
			}
		}
    
    void expanding_definition::output(ostream& o) const
    {
			o << '{' << _name << '}';
    }

		const lax expanding_definition::bound_to(const lax& a) const
		{
			return (recursing || !_) ? value::bound_to(a) : evaluated_at(a);
		}
	}
}
