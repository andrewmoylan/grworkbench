#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <lift/sqr.hpp>
#include "bulirsch_stoer_parameters.hpp"

namespace grwb
{
  
  

  template <typename T> class stoermer_stepper
  {
  public:
    stoermer_stepper(const function<optional<T> (const double&, const T&)>& f, const double& x_0, const T& y_0) : 
      f_(f),
  	  x_0_(x_0),
	    y_0_(y_0),
      f_y_0_(f(x_0, y_0))
    {
    }

    optional<T> step(const double& total_h, const size_t& steps) const
    {
      // adapted from Numerical Recipes in C (2nd Edition), p. 733

      optional<T> ret;

      if (!f_y_0_)
	return ret;

      const double h(total_h / double(steps));
      const double half_h(0.5 * h);
      const double h_squared(sqr(h));

      double x(x_0_ + h);
      T y_temp(y_0_);

      y_temp[0] += (y_temp[1] = h * (y_0_[1] + half_h * (*f_y_0_)[1]));

      optional<T> dydx(f_(x, y_temp));
      if (!dydx)
	return ret;

      for (size_t i(1); i < steps; ++i)
      {
        y_temp[0] += (y_temp[1] += h_squared * (*dydx)[1]);
        dydx = f_(x += h, y_temp);
        if (!dydx)
          return ret;
      }

      (y_temp[1] /= h) += half_h * (*dydx)[1];
      ret.reset(y_temp);
      return ret;
    }

  private:
    const function<optional<T> (const double&, const T&)> f_;
    const double x_0_;
    const T y_0_;
    const optional<T> f_y_0_;
  };

  template <> class bulirsch_stoer_parameters<stoermer_stepper>
  {
  public:    
    const static size_t k_total = 14;
    const static size_t k_values[k_total + 1];
  };
  
  template <> const size_t bulirsch_stoer_parameters<stoermer_stepper>::k_values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
}
