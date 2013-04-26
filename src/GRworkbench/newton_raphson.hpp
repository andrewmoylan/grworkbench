//#pragma once
//
//#include <boost/function.hpp>
//#include <boost/optional.hpp>
//#include "matrix.h"
//
//namespace grwb
//{
//  
//  class quick_jacobian
//  {
//  public:
//    quick_jacobian(const function<optional<nvector<double> > (const nvector<double>&)>& f)
//      : f_(f)
//    {
//    }
//
//    optional<nvector<nvector<double> > > operator()(const nvector<double>& x) const
//    {
//      const size_t n(x.size());
//      
//      optional<nvector<double> > fx(f_(x));
//      if (!fx)
//        return optional<nvector<nvector<double> > >();
//
//      nvector<nvector<double> > result(n, unchanging(*fx));
//      
//      for (size_t i(0); i < n; ++i)
//      {
//        const double h(default_approx_equal_tolerance * x[i]);
//
//        nvector<double> xx(x);
//        xx[i] += h;
//        const optional<nvector<double> > fxx(f_(xx));
//
//        if (!fxx)
//          return optional<nvector<nvector<double> > >();
//
//        result[i] = (*fxx - *fx) / h;
//      }
//
//      return optional<nvector<nvector<double> > >(transpose(result));
//    }
//
//  private:
//    function<optional<nvector<double> > (const nvector<double>&)> f_;
//  };
//
//  class newton_raphson
//  {
//  public:
//    newton_raphson(const function<optional<nvector<double> > (const nvector<double>&)>& f, const double& x_tolerance = default_approx_equal_tolerance, const double& f_tolerance = default_approx_equal_tolerance)
//      : f_(f),
//        x_tolerance_(x_tolerance),
//        f_tolerance_(f_tolerance),
//        jacobian(f)
//    {
//    }
//
//    optional<nvector<double> > operator()(const nvector<double>& x) const
//    {
//      const size_t n(x.size());
//
//      nvector<double> xx(x);
//
//      optional<nvector<double> > f_xx(f_(xx));
//      if (!f_xx)
//        return optional<nvector<double> >();
//
//      for (size_t i(0); i < max_steps; ++i)
//      {
//        optional<nvector<nvector<double> > > inv_j(jacobian(xx));
//        if (!inv_j)
//          return optional<nvector<double> >();
//        try
//        {
//          inv_j.reset(inverse(*inv_j));
//        }
//        catch (...)
//        {
//          //cout << "Newton-Raphson: Jacobian singular." << endl;
//          return optional<nvector<double> >();
//        }
//
//        const nvector<double> delta_xx(*inv_j * *f_xx);
//        xx -= delta_xx;
//
//        const optional<nvector<double> > old_f_xx(f_xx);
//        f_xx = f_(xx);
//        if (!f_xx)
//          return optional<nvector<double> >();
//
//        if (approx_equal(xx, xx + delta_xx, x_tolerance_) || approx_equal(*f_xx, *old_f_xx, f_tolerance_))
//          return optional<nvector<double> >(xx);
//      }
//      
//      return optional<nvector<double> >();
//    }
//    
//  private:
//    const function<optional<nvector<double> > (const nvector<double>&)> f_;
//    const quick_jacobian jacobian;
//    const double f_tolerance_;
//    const double x_tolerance_;
//
//    static const int max_steps = 100;
//  };
//}
