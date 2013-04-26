#pragma once

#include "name_space.hpp"

namespace lax_interpreter
{	
	class engine
	{
	public:
		static void begin_call();
		static void end_call();
		static void reset();
		static void set_call_limit(const int);

		static int& side_effects();

		static parser::name_space*& current_context();

	private:
		~engine();

		static int& call_count();
		static int& call_limit();
	};
}
