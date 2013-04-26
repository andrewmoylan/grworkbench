//#include "curve.hpp"
//#include "point.hpp"
//#include "opengl.hpp"
//#include "view.hpp"
//
//namespace grwb
//{
//  curve::curve(const shared_ptr<function<optional<shared_ptr<point> > (const double&)> >& f, const double& a, const double& b, const int s, const vector<double, 3>& colour)
//		: _(f),
//		begin(a),
//		end(b),
//		samples(s),
//		colour_(colour)
//	{
//	}
//
//	void curve::draw(const view& v) const
//	{
//    const double delta = (end - begin) / samples;
//    const double epsilon = delta / 2.;
//    optional<shared_ptr<point> > a;
//    optional<shared_ptr<point> > b((*_)(begin));
//    optional<vector<double, 3> > fa;
//    optional<vector<double, 3> > fb;
//    if (b)
//      fb = v(*b);
//    glBegin(GL_LINES);
//    glColor3d(colour_[0], colour_[1], colour_[2]);
//    for (double i(begin + delta); i < end + epsilon; i += delta)
//    {
//      a = b;
//      fa = fb;
//      b = (*_)(i);
//      if (b)
//        fb = v(*b);
//      else
//        fb = optional<vector<double, 3> >();
//      recursive_draw(i - delta, i, fa, fb, v, 0);
//    }
//    glEnd();
//		glColor3d(1., 1., 1.);
//  }
//
//  template <typename U> bool colinear(const vector<U, 3>& a, const vector<U, 3>& b, const vector<U, 3>& c)
//  {
//    const double cosine_threshold = 0.9975;
//
//    vector<U, 3> p(b - a);
//    vector<U, 3> q(c - b);
//
//    return cosine_threshold < inner_product(p, q) / (abs(p) * abs(q));
//  }
//
//  using std::abs;
//
//  void curve::recursive_draw(const double& a, const double& b, const optional<vector<double, 3> >& fa, const optional<vector<double, 3> >& fb, const view& v, size_t recursion) const
//  {
//    if (recursion > maximum_recursion)
//    {
//      if (fa && fb)
//      {
//        glColor3d(1., 0., 0.);
//        glVertex3dv(fa->begin());
//        glVertex3dv(fb->begin());
//        glColor3d(colour_[0], colour_[1], colour_[2]);
//      }
//    }
//    else
//    {
//      optional<shared_ptr<point> > c((*_)((a + b) / 2.));
//      optional<vector<double, 3> > fc;
//      if (c)
//        fc = optional<vector<double, 3> >(v(*c));
//      if (fa && fb && fc && colinear(*fa, *fc, *fb))
//      {
//        glVertex3dv(fa->begin());
//        glVertex3dv(fc->begin());
//        glVertex3dv(fc->begin());
//        glVertex3dv(fb->begin());
//      }
//      else
//      {
//        recursive_draw(a, (a + b) / 2., fa, fc, v, recursion + 1);
//        recursive_draw((a + b) / 2., b, fc, fb, v, recursion + 1);
//      }
//    }
//  }
//
//}
