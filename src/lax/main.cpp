#include <iostream>
#include "parse.hpp"
#include "name_space.hpp"
#include "log_cout.hpp"

using namespace std;

int main(int, char**)
{
	system("title lax intepreter");

	lax_interpreter::parser::top_level_name_space context(true);
	lax_interpreter::parser::include_file("prelude.lax", context);
	cout << endl << "lax interpreter started.  Type 'help' for help." << endl << endl;

	while (true)
	{
		lax_interpreter::log_cout << ">> " << flush;
		string line;
		getline(cin, line);
		if (lax_interpreter::enable_log) lax_interpreter::log << line << endl;

		lax_interpreter::parser::parse_and_evaluate(line, context, false);
	}

	return EXIT_SUCCESS;
}
