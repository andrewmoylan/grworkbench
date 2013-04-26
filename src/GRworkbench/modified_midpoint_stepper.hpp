#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <lift/vector.hpp>
#include "bulirsch_stoer_parameters.hpp"
#include "nvector.hpp"

namespace grwb
{
  
  using boost::function;
  using boost::optional;

  template <class T> class modified_midpoint_stepper
  {
  public:
    modified_midpoint_stepper(const function<optional<T> (const double&, const T&)>& f, const double& x_0, const T& y_0)
      : f_(f),
	x_0_(x_0),
	y_0_(y_0),
	f_y_0_(f(x_0, y_0))
    {
    }

    optional<T> step(const double& total_h, const size_t& steps) const
    {
      // adapted from Numerical Recipes in C (2nd Edition), p. 724
      
      optional<T> ret;

      if (!f_y_0_)
	return ret;

      const double h(total_h / double(steps));
      const double two_h(2. * h);

      T ym(y_0_), yn(y_0_ + h * *f_y_0_);
      double x(x_0_ + h);

      optional<T> dydx(f_(x, yn));
      if (!dydx)
	return ret;
      
      for (size_t i(1); i < steps; ++i)
      {
	T y_next(ym + two_h * *dydx);
	ym = yn;
	
        dydx = f_(x += h, yn = y_next);
	if (!dydx)
	  return ret;
      }

      ret.reset(0.5 * (ym + yn + h * *dydx));
      return ret;
    }

  private:
    const function<optional<T> (const double&, const T&)> f_;
    const double x_0_;
    const T y_0_;
    const optional<T> f_y_0_;
    modified_midpoint_stepper& operator=(const modified_midpoint_stepper&);
  };

  template <> class bulirsch_stoer_parameters<modified_midpoint_stepper>
  {
  public:
    const static size_t k_total = 10;
    static size_t k_values[k_total + 1];
    bulirsch_stoer_parameters()
    {
      size_t kv[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22};
      std::copy(kv, kv + k_total + 1, k_values);
    }
  };

}

