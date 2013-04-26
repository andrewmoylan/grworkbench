//#pragma once
//
//#include <boost/optional.hpp>
//#include "atlas.hpp"
//#include "point.hpp"
//#include "worldline.hpp"
//
//namespace grwb
//{
//  class coordinate_line_functor
//  {
//  public:
//    explicit coordinate_line_functor(const shared_ptr<chart>& c, const shared_ptr<point> & x, const nvector<double>& v)
//      : c_(c),
//        a_(x.context()),
//        x_(*x[c]),
//        v_(v)
//    {
//    }
//
//    optional<shared_ptr<point> > operator()(const double& t) const
//    {
//      optional<shared_ptr<point> > result;
//
//      const nvector<double> x(x_ + t * v_);
//      if ((*c_)(x))
//        result.reset(shared_ptr<point>(new point(a_, c_, x)));
//
//      return result;
//    }
//
//  private:
//    const shared_ptr<chart> c_;
//    const shared_ptr<atlas> a_;
//    const nvector<double> x_;
//    const nvector<double> v_;
//  };
//
//  inline cached_worldline coordinate_line(const shared_ptr<point> & x, const shared_ptr<chart>& c, const nvector<double>& v)
//  {
//    return cached_worldline(coordinate_line_functor(c, x, v));
//  }
//
//  inline cached_worldline coordinate_line(const tangent_vector& t, const shared_ptr<chart>& c)
//  {
//    return coordinate_line(t.context(), c, *t[c]);
//  }
//}
