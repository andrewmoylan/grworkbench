#pragma once

#include "lift/elementary.hpp"
#include "brent_minimiser.hpp"
#include "matrix.hpp"

namespace grwb
{
  
  using lift::zero;
  

  template <typename T, typename U> class powell_minimiser
  {
  public:
    powell_minimiser(const function<optional<T> (const U&)>& f)
      : f_(f),
    auto_scale(1.e-2)
    {
    }

    optional<pair<U, T> > operator()(const U& x, const nvector<U>& basis, const double& tolerance = default_approx_equal_tolerance) const
    {
      U minimum(x);
      optional<T> op(f_(minimum));
      if (!op)
        return optional<pair<U, T> >();
      T f_min(*op);
      nvector<U> basis_(basis);

      for (size_t i(0); i < max_steps; ++i)
      {
        const U prev_min(minimum);
        const T prev_f_min(f_min);
        T largest_decrease(zero(f_min));
        size_t largest_decrease_index(0);

        for (size_t j(0); j < basis.size(); ++j)
        {
          optional<pair<double, T> > line_minimum(brent_minimiser(linear_subspace(f_, minimum, basis_[j]))(0., 0., tolerance));
          if (!line_minimum)
            return optional<pair<U, T> >();

          if (f_min - line_minimum->second > largest_decrease)
          {
            largest_decrease = f_min - line_minimum->second;
            largest_decrease_index = j;
          }

          if (zero(line_minimum->first) != line_minimum->first && f_min > line_minimum->second)
          {
            basis_[j] *= line_minimum->first;
            minimum += basis_[j];
            f_min = line_minimum->second;
          }
        }

        if (approx_equal(prev_f_min, f_min, tolerance))
          return optional<pair<U, T> >(make_pair(minimum, f_min));

        const U new_direction(minimum - prev_min);
        const U extrapolated_min(minimum + new_direction);
        op = f_(extrapolated_min);
        if (!op)
          return optional<pair<U, T> >();
        const T f_extrapolated_min(*op);
        
        if (f_extrapolated_min < prev_f_min)
        {
          if (2. * (prev_f_min - 2. * f_min + f_extrapolated_min) * sqr(prev_f_min - f_min - f_extrapolated_min) <= largest_decrease * sqr(prev_f_min - f_extrapolated_min))
          {
            optional<pair<double, T> > line_minimum(brent_minimiser(linear_subspace(f_, minimum, new_direction))(0., 0., tolerance));
            if (!line_minimum)
              return optional<pair<U, T> >();

            if (zero(line_minimum->first) != line_minimum->first && f_min > line_minimum->second)
            {
              basis_[largest_decrease_index] = basis_[basis.size() - 1];
              minimum += (basis_[basis.size() - 1] = new_direction * line_minimum->first);
              f_min = line_minimum->second;
            }
          }
        }
      }

      return optional<pair<U, T> >();
    }

    optional<pair<U, T> > operator()(const U& x, const double& tolerance = default_approx_equal_tolerance) const
    {
      return operator()(x, default_basis(x), tolerance);
    }

  private:
    const function<optional<T> (const U&)> f_;

    const static size_t max_steps = 100;
    const double auto_scale;

    class linear_subspace_functor
    {
    public:
      linear_subspace_functor(const function<optional<T> (const U&)>& f, const U& origin, const U& direction)
        : f_(f),
          origin_(origin),
          direction_(direction)
      {
      }

      optional<T> operator()(const double& t) const
      {
        return f_(origin_ + t * direction_);
      }
      
    private:
      const function<optional<T> (const U&)>& f_;
      const U& origin_;
      const U& direction_;
    };

    function<optional<T> (const double&)> linear_subspace(const function<optional<T> (const U&)>& f, const U& origin, const U& direction) const
    {
      return linear_subspace_functor(f, origin, direction);
    }

    nvector<U> default_basis(const U& x) const
    {
      nvector<U> r(unity(nvector<U>(x.size(), make_constant_iterator(x))));
      for (size_t i(0); i < x.size(); ++i)
      {
        const double scale(auto_scale * abs(x[i]));
        if (scale > 0)
          r[i] *= scale;
      }
      return r;
    }
  };
}
