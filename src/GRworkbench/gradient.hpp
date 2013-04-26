//#pragma once
//
//#include <boost/function.hpp>
//#include <boost/optional.hpp>
//#include <lift/vector.hpp>
//#include "derivative.hpp"
//
//namespace grwb
//{
//  
//  namespace gradient_detail
//  {
//    template <typename T> class single_coordinate_function
//    {
//    public:
//      single_coordinate_function(const function<optional<T> (const nvector<double>&)>& f, const nvector<double>& x, const size_t& i)
//	: f_(f),
//	  x_(x),
//	  i_(i)
//      {
//      }
//
//      optional<T> operator()(const double& delta_x_i)
//      {
//	nvector<double> _x(x_);
//	_x[i_] += delta_x_i;
//	return f_(_x);
//      }
//      
//    private:
//      const function<optional<T> (const nvector<double>&)>& f_;
//      const nvector<double>& x_;
//      const size_t& i_;
//    };
//  }
//
//  template <typename T> class gradient_functor
//  {
//  public:
//    gradient_functor(function<optional<T> (const nvector<double>&)>f)
//      : f_(f)
//    {
//    }
//
//    optional<nvector<T> > operator()(const nvector<double>& x)
//    {
//      const optional<T> default_value(f_(x));
//
//      if (!default_value)
//        return optional<nvector<T> >();
//
//      nvector<T> result(x.size(), make_constant_iterator(*default_value));
//
//      for (size_t i = 0; i != x.size(); ++i)
//      {
//        optional<T> d(derivative<T>(gradient_detail::single_coordinate_function<T>(f_, x, i))(0.));
//        if (!d)
//          return optional<nvector<T> >();
//        result[i] = *d;
//      }
//
//      return optional<nvector<T> >(result);
//    }
//
//  private:
//    const function<optional<T> (const nvector<double>&)> f_;
//  };
//
//  template<typename T> function<optional<nvector<T> >(const nvector<double>&)> gradient(const function<optional<T>(const nvector<double>&)>& f)
//  {
//    return gradient_functor<T>(f);
//  }
//}
