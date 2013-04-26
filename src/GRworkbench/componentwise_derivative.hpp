//#pragma once
//
//#include <lift/vector.h>
//#include "derivative.h"
//
//namespace grwb
//{
//
//  namespace componentwise_derivative_detail
//  {
//    template <typename T> class nvector_component_functor
//    {
//    public:
//      nvector_component_functor(const function<optional<nvector<T> > (const double&)>& f, size_t i)
//	: f_(f),
//	  i_(i)
//      {
//      }
//
//      optional<T> operator()(const double& x) const
//      {
//	optional<nvector<T> > v = f_(x);
//	return v ? optional<T>((*v)[i_]) : optional<T>();
//      }
//
//    private:
//      const function<optional<nvector<T> > (const double&)> f_;
//      const size_t i_;
//    };
//  }
//
//  template <typename T> class derivative_functor<nvector<T> >
//  {
//  public:
//    derivative_functor(const function<optional<nvector<T> > (const double&)>& f, const double& scale, const double& tolerance)
//      : f_(f),
//	scale_(scale),
//	tolerance_(tolerance)
//    {
//    }
//
//    optional<nvector<T> > operator()(const double& x) const
//    {
//      optional<nvector<T> > default_value = f_(x);
//      if (!default_value)
//	return default_value;
//
//      nvector<T> result(*default_value);
//      
//      for (size_t  i = 0; i < result.size(); ++i)
//      {
//        optional<T> dd(derivative(function<optional<T> (const double&)>(componentwise_derivative_detail::nvector_component_functor<T>(f_, i)), scale_, tolerance_)(x));
//        if (!dd)
//          return optional<nvector<T> >();
//        result[i] = *dd;
//      }
//
//      return optional<nvector<T> >(result);
//    }
//
//  private:
//    function<optional<nvector<T> > (const double&)> f_;
//    const double scale_;
//    const double tolerance_;
//  };
//}
