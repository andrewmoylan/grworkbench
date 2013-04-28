#pragma once

#include <string>
#include <boost/optional.hpp>
#include "lax.hpp"
#include "name_space.hpp"

namespace lax_interpreter
{
	namespace parser
	{
		
		const boost::optional<const lax> parse(const std::string&, name_space&);
		void parse_and_evaluate(const std::string&, name_space&, const bool silent);
		void include_file(const std::string&, name_space&);
	}
}
