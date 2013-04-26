#pragma once

#include <boost/shared_ptr.hpp>
#include "value.hpp"

namespace lax_interpreter
{
	namespace values
	{
		class boolean	:	public value
		{
		public:
			virtual ~boolean();

			static const shared_ptr<const boolean> make(const bool);

      virtual void output(ostream&) const;

      virtual const bool is_boolean() const;
      virtual const bool get_boolean() const;

    private:
      boolean();
      boolean(const boolean&);
      boolean& operator=(const boolean&);

			explicit boolean(const bool);

      const bool _;
    };

    class character
			: public value
    {
    public:
      virtual ~character();

      static const shared_ptr<const character> make(const char);

      virtual void output(ostream&) const;

      virtual const bool is_character() const;
      virtual const char get_character() const;

    private:
      character();
      character(const character&);
      character& operator=(const character&);

      explicit character(const char);

      const char _;
    };

    class integer 
			: public value
    {
    public:
      virtual ~integer();

      static const shared_ptr<const value> make(const int);

      virtual void output(ostream&) const;

      virtual const bool is_integer() const;
			virtual const int get_integer() const;
      
      virtual const bool is_real() const;
			virtual const double get_real() const;
      
    private:
      integer();
      integer(const integer&);
      integer& operator=(const integer&);

			explicit integer(const int);

      const int _;
    };
    
    class real 
			: public value
    {
    public:
      virtual ~real();

      static const shared_ptr<const value> make(const double&);

      virtual void output(ostream&) const;

      virtual const bool is_real() const;
      virtual const double get_real() const;

    private:
      real();
      real(const real&);
      real& operator=(const real&);

			explicit real(const double);

      const double _;
    };
  }
}

