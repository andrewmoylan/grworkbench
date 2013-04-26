//#pragma once
//
//#include <boost/function.hpp>
//#include <boost/optional.hpp>
//#include "relative_difference.hpp"
//#include "richardson_extrapolation.hpp"
//
//namespace grwb
//{
//
//  template <typename T> class derivative_functor
//  {
//  public:
//    derivative_functor(const function<optional<T> (const double&)>& f, const double& scale, const double& tolerance)
//      : f_(f),
//	scale_(scale),
//	tolerance_(tolerance),
//       max_steps_(13),
//     step_scale_(1.7)
//
//    {
//    }
//
//    optional<T> operator()(const double& x) const
//    {
//      if (!f_(x))
//	return optional<T>();
//      
//      double h(scale_);
//
//      optional<richardson_extrapolation<T> > extrapolator;
//
//      for (size_t i(0); i < max_steps_; ++i)
//      {
//	const optional<T> right(f_(x + h));
//	const optional<T> left(f_(x - h));
//	if (left && right)
//	{ 	  
//	  const T diff((*right - *left) / (2. * h));
//
//	  if (!extrapolator)
//	    extrapolator.reset(richardson_extrapolation<T>(h * h, diff));
//	  else
//	  {
//	    extrapolator->refine(h * h, diff);
//	    if (tolerance_ > relative_difference(extrapolator->limit(), extrapolator->limit() + extrapolator->error()))
//	      return optional<T>(extrapolator->limit());
//	  }
//	}
//
//        h /= step_scale_;
//      }
//
//      return optional<T>();
//    }
//
//  private:
//    const function<optional<T> (const double&)> f_;
//    const double scale_;
//    const double tolerance_;
//
//    const size_t max_steps_;
//    const double step_scale_;
//  };
//
//  template <typename T> function<optional<T> (const double&)> derivative(const function<optional<T> (const double&)>& f, const double& scale = 1., const double& tolerance = default_approx_equal_tolerance)
//  {
//    return derivative_functor<T>(f, scale, tolerance);
//  }
//}
