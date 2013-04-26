#include <iostream>
#include "boost/current_function.hpp"
#include "value.hpp"
#include "lax.hpp"
#include "builtins.hpp"
#include "call.hpp"

namespace lax_interpreter
{
  namespace values
  {
    value::value()
    {
    }

    value::~value()
    {
    }
 
    const lax value::evaluated() const
    {
			return lax(shared_from_this());
    }

    const lax value::evaluated_at(const lax&) const
    {
			return builtins::nil();
    }

    const bool value::is_boolean() const
    {
      return false;
    }

    const bool value::is_character() const
    {
      return false;
    }

    const bool value::is_integer() const
    {
      return false;
    }

    const bool value::is_real() const
    {
      return false;
    }

    const bool value::get_boolean() const
    {
			throw "Invalid call to const bool value::get_boolean() const: value was not a bool.";
    }

    const char value::get_character() const
    {
      throw "Invalid call to const char value::get_character() const: value was not a char.";
    }

    const int value::get_integer() const
    {
      throw "Invalid call to const int value::get_integer() const: value was not an int.";
    }

    const double value::get_real() const
    {
      throw "Invalid call to const double value::get_real() const: value was not a double.";
    }

		const lax value::get_left() const
		{
			return builtins::I();
		}

		const lax value::get_right() const
		{
			return lax(shared_from_this());
		}

		const lax value::bound_to(const lax& r) const
		{
			return call::make(lax(shared_from_this()), r);
		}

		const bool value::is_call() const
		{
			return false;
		}
  }
}

