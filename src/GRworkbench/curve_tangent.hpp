//#pragma once
//// boost
//#include <boost/function.hpp>
//#include <boost/optional.hpp>
//#include <boost/shared_ptr.hpp>
//
//namespace grwb
//{
//
//  class tangent_vector;
//  class point;
//
//  class curve_tangent
//  {
//  public:
//    curve_tangent(function<optional<shared_ptr<point> > (const double&)> f);
//    optional<tangent_vector> operator()(const double& x) const;
//
//  private:
//    const function<optional<shared_ptr<point> > (const double&)> f_;
//  };
//}
//
//// grwb
//#include "point.h"
//#include "tensor.h"
//#include "derivative.h"
//
//namespace grwb
//{
//
//  inline curve_tangent::curve_tangent(function<optional<shared_ptr<point> > (const double&)> f)
//    : f_(f)
//  {
//  }
//
//  namespace curve_tangent_detail
//  {
//    class curve_on_chart
//    {
//    public:
//      curve_on_chart(const function<optional<shared_ptr<point> > (const double&)>& f, const shared_ptr<chart>& c)
//	: f_(f),
//	  chart_(c)
//      {
//      }
//
//      optional<nvector<double> > operator()(const double& x) const
//      {
//	optional<shared_ptr<point> > p(f_(x));
//	return p ? (*p)[chart_] : optional<nvector<double> >();
//      }
//
//    private:
//      const function<optional<shared_ptr<point> > (const double&)> f_;
//      const shared_ptr<chart> chart_;
//    };
//  }
//
//  inline optional<tangent_vector> curve_tangent::operator()(const double& x) const
//  {
//    optional<shared_ptr<point> > p = f_(x);
//    if (!p)
//      return optional<tangent_vector>();
//
//    const shared_ptr<chart>& c(p->valid_chart());
//
//    return optional<tangent_vector>(tangent_vector(*p, c, *derivative<nvector<double> >(function<optional<nvector<double> > (const double&)>(curve_on_chart(f_, c)))(x)));
//  }
//}
//
//#endif
