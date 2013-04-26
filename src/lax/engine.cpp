#include <string>
#include <sstream>
#include <iostream>
#include "engine.hpp"

namespace lax_interpreter
{
	using namespace std;

	void engine::begin_call()
	{
		if (++call_count() > call_limit())
		{
			stringstream ss;
			ss << "Recursion limit of " << call_limit() << " calls reached";
			throw ss.str();
		}
	}

	void engine::end_call()
	{
		--call_count();
	}

	int& engine::call_limit()
	{
		static int _(8096);
		return _;
	}

	int& engine::call_count()
	{
		static int _(0);
		return _;
	}

	int& engine::side_effects()
	{
		static int _(0);
		return _;
	}

	void engine::reset()
	{
		call_count() = 0;
	}

	void engine::set_call_limit(const int n)
	{
		call_limit() = n;
	}

	parser::name_space*& engine::current_context()
	{
		static parser::name_space* _(0);
		return _;
	}
}
