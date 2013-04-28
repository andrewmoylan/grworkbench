#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <memory>
#include <lift/vector.hpp>
#include "approx_equal.hpp"
#include "colinear.hpp"
#include "object.hpp"
#include "opengl_compound_list.hpp"

namespace grwb
{
  using std::pair;
  using boost::function;
  using boost::optional;
  using lift::vector;

  class point;
  class view;

  template <class T> class curve
        : public object, public enable_shared_from_this<curve<T> >
  {

  private:

    struct segment
    {
      segment(const double& a, const double& b, const optional<vector<double, 3> >& fa, const optional<vector<double, 3> >& fb) : _a(a), _b(b), _fa(fa), _fb(fb) {}
      segment(const double& a, const double& b) : _a(a), _b(b) {}
      double _a;
      double _b;
      optional<vector<double, 3> > _fa;
      optional<vector<double, 3> > _fb;
    };

  private:
    mutable list<segment> _finalised;
    mutable list<segment> _intermediate;
    mutable list<segment> _unevaluated_intermediate;

    struct idle
    {
      idle(const std::shared_ptr<curve>& c) : _c(c) {}
      const weak_ptr<curve> _c;
      void operator()() const
      {
        if (std::shared_ptr<curve> c = _c.lock())
          if (c->refine())
            application::push(*this);
      }
      idle& operator=(const idle&);
    };

  public:
    curve(const std::shared_ptr<function<optional<T> (const double&)> >& f, const double& a, const double& b, const int s, const vector<double, 3>& colour, view& v)
        : _(f),
        begin(a),
        end(b),
        samples(s),
        colour_(colour),
        object(v)
  {}

    void do_interval(const double& a, const double& b, const int s)
    {
      for (int i(0); i != s; ++i)
      {
        const double l(a + (b - a) * i / s);
        const double r(a + (b - a) * (i + 1) / s);
        _unevaluated_intermediate.push_back(segment(l, r));
      }
    }

    void freeze()
    {
      _intermediate.clear();
      _unevaluated_intermediate.clear();
    }

    bool refine() const
    {
			const static double safe(1.e-6);
      
			if (!_unevaluated_intermediate.empty())
      {
        segment x(_unevaluated_intermediate.front());
        _unevaluated_intermediate.pop_front();

        {
          optional<T> c((*_)(x._a));
          if (c) x._fa = view_(c);/* else cout <<"asdf1"<<endl;*/
        }
        {
          optional<T> c((*_)(x._b));
          if (c) x._fb = view_(c);/* else cout <<"asdf"<<endl;*/
        }

        _intermediate.push_back(x);

        return true;
      }
			else if (!_intermediate.empty())
      {
        segment x(_intermediate.front());
        _intermediate.pop_front();

        double x_c((x._a + x._b) * .5);
        optional<T> c((*_)(x_c));
        optional<vector<double, 3> > fc(view_(c));

        if (x._fa && x._fb && fc && ((abs((*x._fa + *x._fb) * .5 - *fc) < safe) || colinear(*x._fa, *fc, *x._fb)))
        {
          _finalised.push_back(x);
        }
        else
        {
          if (x._fa || fc)
          {
            if (abs(x._a - x_c) > safe)
              _intermediate.push_back(segment(x._a, x_c, x._fa, fc));
            //else
            //cout << "Won't draw curve beyond " << x._a << endl;
          }
					//else cout << "between " << x._a << " and " << x_c << endl;
          if (fc || x._fb)
          {
            if (abs(x._b - x_c) > safe)
              _intermediate.push_back(segment(x_c, x._b, fc, x._fb));
            //else
            //cout << "Won't draw curve beyond " << x._b << endl;
          }
					//else cout << "zbetween " << x_c << " and " << x._b << endl;
        }
        return true;
      }
			else
      {
        return false;
      }
    }

  private:
    mutable opengl_compound_list display_lists;

  public:
    virtual void invalidate()
    {
      if (_intermediate.empty() && _unevaluated_intermediate.empty())
        application::push(idle(this->shared_from_this()));
      clear();
      do_interval(begin, end, samples);
    }

    virtual void draw() const
    {
      display_lists.draw();

      const GLint new_list((_finalised.size() >= 25 || (_finalised.size() >= 1 && _intermediate.empty() && _unevaluated_intermediate.empty())) ? glGenLists(1) : 0);
      if (new_list)
        glNewList(new_list, GL_COMPILE_AND_EXECUTE);
      glBegin(GL_LINES);
      glColor3d(colour_[0], colour_[1], colour_[2]);
      for (typename list<segment>::const_iterator i(_finalised.begin()); i != _finalised.end(); ++i)
      {
        /*glVertex3dv((*v(*(*_)(i->_a))).begin());
        glVertex3dv((*v(*(*_)(i->_b))).begin());*/
        glVertex3dv(i->_fa->begin());
        glVertex3dv(i->_fb->begin());
      }
      glEnd();
      if (new_list)
      {
        glEndList();
        display_lists.push_back(new_list);
        _finalised.clear();
      }

      // red lines for non-smooth bits
			glBegin(GL_LINES);
      glColor3d(1., 0., 0.);
      for (typename list<segment>::const_iterator j(_intermediate.begin()); j != _intermediate.end(); ++j)
      {
        if (j->_fa && j->_fb)
        {
          glVertex3dv(j->_fa->begin());
          glVertex3dv(j->_fb->begin());
        }
      }
      glEnd();
    }

    void clear()
    {
      display_lists.clear();
      _intermediate.clear();
      _finalised.clear();
      _unevaluated_intermediate.clear();
    }

    void update(const double& new_begin, const double& new_end, const int new_samples, const vector<double, 3>& new_colour)
    {
      bool get_cpu(false);

      if (colour_ != new_colour || new_samples > samples || !(new_begin <= begin && new_end >= end))
      {
        get_cpu = _intermediate.empty() || _unevaluated_intermediate.empty();
        clear();
        do_interval(new_begin, new_end, new_samples);
      }
      else
      {
        if (new_begin < begin)
        {
          do_interval(new_begin, begin, 2 + int(double(new_samples) * (begin - new_begin) / (new_end - new_begin)));
          get_cpu = true;
        }
        if (new_end > end)
        {
          do_interval(end, new_end, 2 + int(double(new_samples) * (new_end - end) / (new_end - new_begin)));
          get_cpu = true;
        }
      }

      if (get_cpu)
        application::push(idle(this->shared_from_this()));

      colour_ = new_colour;
      begin = new_begin;
      end = new_end;
      samples = new_samples;
    }

  private:
    double begin;
    double end;
    int samples;
    vector<double, 3> colour_;

    const std::shared_ptr<function<optional<T> (const double&)> > _;
    curve& operator=(const curve&);
  };
}
