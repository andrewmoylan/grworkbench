#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

namespace lax_interpreter
{
	using namespace std;

	class log_cout_buf : public stringbuf
	{
		friend class log_cout_stream;
	protected:
		virtual int sync();

	private:
		log_cout_buf();
		virtual ~log_cout_buf();
	};

	class log_cout_stream : public ostream
	{
	public:
		log_cout_stream();
		virtual ~log_cout_stream();
	};

	extern log_cout_stream log_cout;
	extern ofstream log;
	extern bool enable_log;
}
