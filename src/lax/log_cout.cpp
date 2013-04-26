#include "log_cout.hpp"
#include <string>

namespace lax_interpreter
{
	log_cout_stream log_cout;
	ofstream log("log", ios::app);
	bool enable_log(false);

	log_cout_buf::~log_cout_buf()
	{
		sync();
	}

	int log_cout_buf::sync()
	{
		cout << str() << flush;
		if (enable_log)
			log << str() << flush;

		str(string());
		return 0;
	}

	log_cout_buf::log_cout_buf()
	{
	}

	log_cout_stream::log_cout_stream()
		: ostream(new log_cout_buf())
	{
	}

	log_cout_stream::~log_cout_stream()
	{
		delete rdbuf();
	}
}
