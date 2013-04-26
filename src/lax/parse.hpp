#pragma once

#include <string>
#include <boost/optional.hpp>
#include "lax.hpp"
#include "name_space.hpp"

namespace lax_interpreter
{
	namespace parser
	{
		using namespace std;

		const optional<const lax> parse(const string&, name_space&);
		void parse_and_evaluate(const string&, name_space&, const bool silent);
		void include_file(const string&, name_space&);
	}
}
