#pragma once

#include <map>
#include "lax.hpp"

#define DECLARE_BUILTIN(NAME) \
	const lax& NAME()

namespace lax_interpreter
{
	namespace builtins
	{
		using namespace std;
		
		const map<string, lax>& builtins();

		DECLARE_BUILTIN(quotation_mark);

		DECLARE_BUILTIN(set_logging);

		DECLARE_BUILTIN(nil);

		DECLARE_BUILTIN(list);

		DECLARE_BUILTIN(get_dynamic_variable);
		DECLARE_BUILTIN(set_dynamic_variable);

		DECLARE_BUILTIN(left);
		DECLARE_BUILTIN(right);
		DECLARE_BUILTIN(head);
		DECLARE_BUILTIN(tail);

		DECLARE_BUILTIN(sine);
		DECLARE_BUILTIN(absolute_value);
		DECLARE_BUILTIN(arc_cosine);
		DECLARE_BUILTIN(arc_sine);
		DECLARE_BUILTIN(arc_tangent);
		DECLARE_BUILTIN(ceiling);
		DECLARE_BUILTIN(cosine);
		DECLARE_BUILTIN(hyperbolic_cosine);
		DECLARE_BUILTIN(exponential);
		DECLARE_BUILTIN(floor);
		DECLARE_BUILTIN(logarithm);
		DECLARE_BUILTIN(hyperbolic_sine);
		DECLARE_BUILTIN(square_root);
		DECLARE_BUILTIN(tangent);
		DECLARE_BUILTIN(hyperbolic_tangent);

		DECLARE_BUILTIN(power);
		DECLARE_BUILTIN(arc_tangent_2);

		DECLARE_BUILTIN(equal);
		DECLARE_BUILTIN(inequal);
		DECLARE_BUILTIN(identical);
		DECLARE_BUILTIN(nonidentical);

		DECLARE_BUILTIN(if_then_else);

		DECLARE_BUILTIN(I);
		DECLARE_BUILTIN(K);
		DECLARE_BUILTIN(S);

		DECLARE_BUILTIN(Kn);
		DECLARE_BUILTIN(Sn);
		DECLARE_BUILTIN(K_after_n);

		DECLARE_BUILTIN(add);
		DECLARE_BUILTIN(subtract);
		DECLARE_BUILTIN(multiply);
		DECLARE_BUILTIN(divide);

		DECLARE_BUILTIN(negate);

		DECLARE_BUILTIN(logical_not);
		DECLARE_BUILTIN(logical_and);
		DECLARE_BUILTIN(logical_or);

		DECLARE_BUILTIN(less_than);
		DECLARE_BUILTIN(greater_than);
		DECLARE_BUILTIN(less_than_or_equal);
		DECLARE_BUILTIN(greater_than_or_equal);

		DECLARE_BUILTIN(is_boolean);
		DECLARE_BUILTIN(is_character);
		DECLARE_BUILTIN(is_integer);
		DECLARE_BUILTIN(is_real);

		DECLARE_BUILTIN(show);
		DECLARE_BUILTIN(show_string);
		DECLARE_BUILTIN(showed);
		DECLARE_BUILTIN(showed_string);

		DECLARE_BUILTIN(include_file);

		DECLARE_BUILTIN(throw_exception);

		DECLARE_BUILTIN(evaluated);

		DECLARE_BUILTIN(numeric_range);

		DECLARE_BUILTIN(current_directory);
		DECLARE_BUILTIN(change_directory);
		DECLARE_BUILTIN(system_command);
		DECLARE_BUILTIN(is_microsoft_windows);
	}
}

#undef DECLARE_BUILTIN
