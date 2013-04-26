#pragma once

#include "function.hpp"

#define DEFINE_LAX_BUILTIN(TYPE, FUNCTION, NAME, EAGER) \
	const ::lax_interpreter::lax& NAME() \
{ \
	static const ::lax_interpreter::lax _(::lax_interpreter::values::function::make<TYPE>(FUNCTION, #NAME, EAGER)); \
	return _; \
}
