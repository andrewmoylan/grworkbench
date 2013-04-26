#include <iostream>
#include <iomanip>
#include <string>
#include <boost/optional.hpp>
#include "lax/builtins.hpp"
#include "lax/engine.hpp"
#include "lax/parse.hpp"
#include "application.hpp"
#include "lax_builtins.hpp"
#include "window.hpp"
#include "lax/log_cout.hpp"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#  include <X11/Xlib.h>
#endif

namespace grwb
{
	using std::string;
	using boost::optional;

	application::key::state_type application::key::state;

#ifdef _WIN32
	CRITICAL_SECTION application::_cs;
#else
	pthread_mutex_t application::_pm;
#endif

	int application::execute() { return _get()._execute(); }

	using std::cerr;
	using std::endl;

	application::application() : _console(), _context(true)
	{
	}

  application::~application()
  {
    // release mutexes, etc.?
  }

#ifdef _WIN32

	DWORD WINAPI application::_input(void*)
#else
	void* application::_input(void*)
#endif
	{
		for (;;)
		{
			string s;
			getline(std::cin, s);
#ifdef _WIN32
			EnterCriticalSection(&_cs);
#else
			pthread_mutex_lock(&_pm);
#endif
			_get()._console.push_back(s);
#ifdef _WIN32
			LeaveCriticalSection(&_cs);
#else
			pthread_mutex_unlock(&_pm);
#endif
			if (s == "quit" || s == "exit") break;
		}
		return 0;
	}

	int application::_execute()
	{
		using std::endl;
#ifdef _WIN32
		InitializeCriticalSection(&_cs);
		system("title GRworkbench");
		CreateThread(0, 0, _input, 0, 0, 0);
#else
		pthread_mutex_init(&_pm, 0);
		pthread_t t;
		pthread_create(&t, 0, _input, 0);
		window::dpy = XOpenDisplay(NULL);
		if (window::dpy == NULL)
		{
			cerr << "Can't open display" << endl;
			exit(EXIT_FAILURE);
		}
#endif
		lax_interpreter::log_cout << endl << std::setprecision(15) << "Loading GRworkbench builtins ... " << endl;
		_context.insert_map(lax_integration::builtins::builtins());
		lax_interpreter::parser::include_file("prelude.lax", _context);
		lax_interpreter::log_cout << endl << "lax interface to GRworkbench started." << endl;
		std::cout << "Type 'grwb_help' for some help on the lax interface to GRworkbench." << endl;
		std::cout << "Type 'help' for help on lax." << endl;
		lax_interpreter::log_cout << endl << ">> " << std::flush;

		for (;;)
		{
#ifdef _WIN32
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					exit(EXIT_SUCCESS);
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
#else
			XEvent event;
			while (XPending(window::dpy))
			{
				XNextEvent(window::dpy, &event);
				window::MainWndProc(event);
			}
#endif
			optional<string> code;

#ifdef _WIN32
			EnterCriticalSection(&_cs);
#else
			pthread_mutex_lock(&_pm);
#endif
			if (!_console.empty())
			{
				code.reset(_console.front());
				_console.pop_front();
			}
#ifdef _WIN32
			LeaveCriticalSection(&_cs);
#else
			pthread_mutex_unlock(&_pm);
#endif

			if (code)
			{
				if (*code == "exit" || *code == "quit")
					exit(EXIT_SUCCESS);

				if (lax_interpreter::enable_log)
					lax_interpreter::log << *code << endl;

				lax_interpreter::parser::parse_and_evaluate(*code, _context, false);

				lax_interpreter::log_cout << ">> " << std::flush;
			}

			const int t(timeGetTime());

			try
			{
				const size_t n(_timed.size());
				list<pair<int, function<void ()> > >::iterator j(_timed.begin());
				for (size_t i(0); i != n; ++i)
				{
					if (!((t - j->first) & 0x80000000))
					{
						const function<void ()> f(j->second);
						j = _timed.erase(j);
						f();
					}
					else
					{
						++j;
					}
				}

				if (!_idle.empty())
				{
					const function<void ()> f(_idle.front());
					_idle.pop_front();
					f();
				}
			}
			catch (string& s)
			{
				lax_interpreter::log_cout << "Exception while performing background computation: " << s << endl;
			}
			catch (const char* s)
			{
				lax_interpreter::log_cout << "Exception while performing background computation: " << s << endl;
			}
			catch (...)
			{
				lax_interpreter::log_cout << "Unknown exception while performing background computation" << endl;
			}
		}
	}
}

