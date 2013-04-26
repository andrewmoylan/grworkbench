#pragma once

#include <lift/vector.hpp>
#include "application.hpp"
#include "approx_equal.hpp"
#include "colinear.hpp"
#include "object.hpp"
#include "opengl_compound_list.hpp"
#include "view.hpp"
#include "worldline.hpp"

namespace grwb
{
	using lift::vector;
	using lift::make_vector;

  template <class T> class surface  : public object
  {

  public:

		typedef function<optional<T>(const vector<double, 2>&)> surface_type;

		struct idle
    {
      idle(const surface& c) : _c(c) {}
      const surface& _c;
      void operator()() const
      {
        if (_c.refine()) application::push(*this);
      }
    private:
      idle& operator=(const idle&);
    };

		class view_surface
		{
		public:
			view_surface(const surface_type& s, const view& v) : _s(s), _v(v) {}
//			void set_owner(surface& owner) { _ = &owner; }

			optional<vector<double, 3> > operator ()(const vector<double, 2>& x)
			{
				const optional<T> temp(_s(x));
				if (!temp)
				{
					std::cout << "Warning: A surface was not defined for all parameter values for which it was to be visualised." << std::endl;
					return optional<vector<double, 3> >();
				}

				return _v(*temp);
			}

		private:
			const surface_type& _s;
			const view& _v;
//			surface* _;
      view_surface& operator=(const view_surface&);
		};

		surface(view& v, const shared_ptr<surface_type>& p, const vector<double, 2>& a, const vector<double, 2>& b)
			: object(v), _(p), x_range(a), y_range(b), f(*p, v), error(false)
		{
//			f.set_owner(*this);
			init();
			application::push(idle(*this));
		}

		void init()
		{
			{
				const vector<double, 2> l(make_vector(x_range[0], y_range[0]));
				const vector<double, 2> m(make_vector(x_range[0], y_range[1]));
				const vector<double, 2> n(make_vector(x_range[1], y_range[0]));
				optional<vector<double, 3> > fl(f(l));
				optional<vector<double, 3> > fm(f(m));
				optional<vector<double, 3> > fn(f(n));
				if (fl && fm && fn)
					_intermediate.push_back(triangle(l, m, n, *fl, *fm, *fn));
			}
			{
				const vector<double, 2> l(make_vector(x_range[0], y_range[1]));
				const vector<double, 2> m(make_vector(x_range[1], y_range[1]));
				const vector<double, 2> n(make_vector(x_range[1], y_range[0]));
				optional<vector<double, 3> > fl(f(l));
				optional<vector<double, 3> > fm(f(m));
				optional<vector<double, 3> > fn(f(n));
				if (fl && fm && fn)
					_intermediate.push_back(triangle(l, m, n, *fl, *fm, *fn));
			}
		}

		virtual void update(const vector<double, 2>& x_range_, const vector<double, 2>& y_range_)
		{
			x_range = x_range_;
			y_range = y_range_;
			invalidate();
		}

		virtual void invalidate() 
		{
			if (_intermediate.empty())
				application::push(idle(*this));
			_intermediate.clear();
			_finalised.clear();
			display_lists.clear();
			init();
		}

		const bool refine() const
		{
			if (_intermediate.empty() || error)
				return false;

			const triangle t(_intermediate.front());
			_intermediate.pop_front();

			const vector<double, 2>& a(t._a);
			const vector<double, 2>& b(t._b);
			const vector<double, 2>& c(t._c);

			const vector<double, 3>& fa(t._fa);
			const vector<double, 3>& fb(t._fb);
			const vector<double, 3>& fc(t._fc);

			const vector<double, 2> ab((a+b)*.5);
			const vector<double, 2> ac((a+c)*.5);
			const vector<double, 2> bc((b+c)*.5);
			const optional<vector<double, 3> > fab(f(ab));
			const optional<vector<double, 3> > fac(f(ac));
			const optional<vector<double, 3> > fbc(f(bc));
			if (!(fab && fac && fbc))
				return true;
			const vector<double, 3> fafb((fa+fb)*.5);
			const vector<double, 3> fafc((fa+fc)*.5);
			const vector<double, 3> fbfc((fb+fc)*.5);
			const double dab(abs(*fab - fafb));
			const double dac(abs(*fac - fafc));
			const double dbc(abs(*fbc - fbfc));

			if ((colinear(fa, *fab, fb) && colinear(fb, *fbc, fc) && colinear(fa, *fac, fc)) || max(dab, max(dac, dbc)) < 0.01 /*|| min((abs(a - b) * abs(b - c)), min((abs(a - b) * abs(a - c)), (abs(b - c) * abs(a - c)))) < 0.01*/)
				_finalised.push_back(t);
			else
			{
				if (dab >= dbc && dab >= dac)
				{
					// dab
					_intermediate.push_back(triangle(a, ab, c, fa, *fab, fc));
					_intermediate.push_back(triangle(ab, b, c, *fab, fb, fc));
				}
				else if (dbc >= dac)
				{
					// dbc
					_intermediate.push_back(triangle(a, b, bc, fa, fb, *fbc));
					_intermediate.push_back(triangle(a, bc, c, fa, *fbc, fc));
				}
				else
				{
					// dac
					/*log_cout << a << endl << b << endl << c << endl << _intermediate.size() << endl << fa << endl << fb << endl << fc << endl
						<< ac << fac << endl << endl;*/
					_intermediate.push_back(triangle(a, b, ac, fa, fb, *fac));
					_intermediate.push_back(triangle(ac, b, c, *fac, fb, fc));
				}
			}
			return true;
		}

		bool error;

		virtual void draw() const
		{
      glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			display_lists.draw();

			const GLint new_list((_finalised.size() >= 100 || (_intermediate.empty() && _finalised.size() >= 1)) ? glGenLists(1) : 0);
			if (new_list)
				glNewList(new_list, GL_COMPILE_AND_EXECUTE);

			glBegin(GL_TRIANGLES);

			for (typename list<triangle>::const_iterator i(_finalised.begin()); i != _finalised.end(); ++i)
			{
				vector<double, 3> n(cross_product(i->_fb - i->_fa, i->_fc - i->_fa));
				n /= -2.*abs(n);
				n += make_vector(.25, .25, .25);
				glColor4d(n[0], n[1], n[2], 0.35 );

				glVertex3dv(i->_fa.begin());
				glVertex3dv(i->_fb.begin());
				glVertex3dv(i->_fc.begin());
			}
			glEnd();
			if (new_list)
			{
				glEndList();
				display_lists.push_back(new_list);
				_finalised.clear();
			}

			glBegin(GL_TRIANGLES);
			for (typename list<triangle>::const_iterator j(_intermediate.begin()); j != _intermediate.end(); ++j)
			{
				vector<double, 3> n(cross_product(j->_fb - j->_fa, j->_fc - j->_fa));
				n /= -2.*abs(n);
				n += make_vector(.25, .25, .25);
				glColor4d(n[0], n[1], n[2], 0.25);

				glVertex3dv(j->_fa.begin());
				glVertex3dv(j->_fb.begin());
				glVertex3dv(j->_fc.begin());
			}
			glEnd();
		}
    
    vector<double, 2> x_range;
    vector<double, 2> y_range;
//    virtual void draw(const view&) const;

  private:
		mutable view_surface f;

    shared_ptr<surface_type> _;
		mutable opengl_compound_list display_lists;

		struct triangle
    {
      triangle(const vector<double, 2>& a, const vector<double, 2>& b, const vector<double, 2>& c,
				const vector<double, 3>& fa, const vector<double, 3>& fb, const vector<double, 3>& fc)
				: _a(a), _b(b), _c(c), _fa(fa), _fb(fb), _fc(fc) {}
      const vector<double, 2> _a;
			const vector<double, 2> _b;
			const vector<double, 2> _c;
			const vector<double, 3> _fa;
			const vector<double, 3> _fb;
			const vector<double, 3> _fc;
    private:
      triangle& operator=(const triangle&);
    };

		mutable list<triangle> _finalised;
		mutable list<triangle> _intermediate;
    surface& operator=(const surface&);
  };

}
