#pragma once

#include <list>
#include <set>
#include <string>
#include <boost/function.hpp>
#include <lift/array.hpp>
#include "lax/parse.hpp"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#endif

namespace grwb
{
  using std::list;
  using std::string;
  using std::multiset;
  using boost::function;
  using lift::array;
  using std::pair;

  class application
  {
  public:

    static int execute();

    struct key
    {
      enum type
      {
        left = 128,
        right,
        up,
        down,
        enter,
        backspace,
        del,
        end,
        home,
        insert,
        escape,
        tab,
        pageup,
        pagedown,
        f1 = 151,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12
      }; // type

      static class state_type
      {
        friend struct key;
      public:
        array<bool, 256>::reference operator[](const array<bool, 256>::size_type i) { return _get(i); }
        array<bool, 256>::const_reference operator[](const array<bool, 256>::size_type i) const { return _get(i); }
      private:
        state_type() {}
        ~state_type() {}
        static array<bool, 256>::reference _get(const array<bool, 256>::size_type i) { static array<bool, 256> _; return _[i]; }
        state_type(const state_type&);
        state_type& operator=(const state_type&);
      }
      state;

    }
    ; // key

    template<typename F> static void push(const F& f) { _get()._idle.push_back(f); }
    template<typename F> static void push_timed(const int when, const F& f) { _get()._timed.push_back(std::make_pair(when, f)); }

  private:

    application();
    ~application();
    static application& _get() { static application _; return _; }

    int _execute();
 #ifdef _WIN32
    static DWORD WINAPI _input(void*);
 #else
    static void* _input(void*);
 #endif

    list<string> _console;
    lax_interpreter::parser::top_level_name_space _context;
    list<function<void(void)> > _idle;
    list<pair<int, function<void ()> > > _timed;

    application(const application&);
    application& operator=(const application&);

#ifdef _WIN32    
    static CRITICAL_SECTION _cs;
#else
    static pthread_mutex_t _pm;
#endif

  }; // application

} // grwb
