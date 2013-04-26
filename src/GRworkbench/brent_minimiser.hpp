#pragma once

#include <map>
#include <iostream>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <lift/traits.hpp>
#include <lift/vector.hpp>

namespace grwb
{

  using lift::unity;

  template <typename T, typename U> class bracketer
  {

  public:

    bracketer(const function<optional<T> (const U&)>& f)
      : _(f),
    ratio(1.6),
    auto_scale(1.e-2)

    {
    }

    optional<vector<pair<U, T>, 3> > operator()(const U& x, const U& step_size = U()) const
    {
      optional<vector<pair<U, T>, 3> > result;

      optional<T> op(_(x));
      if (!op)
        return result;
      vector<pair<U, T>, 3> r(make_constant_iterator(make_pair(x, *op)));
      U step = step_size == U() ? (x == zero(x) ? unity(x) : auto_scale * abs(x)) : step_size;
      
      r[1].first += step;
      op = _(r[1].first);
      if (!op)
        return result;
      r[1].second = *op;

      if (r[1].second > r[0].second)
      {
        swap(r[0], r[1]);
        step *= -1;
      }

      for (size_t i(0); i < max_steps; ++i)
      {
        r[2].first = r[1].first + step;
        op = _(r[2].first);
        if (!op)
          return result;
        r[2].second = *op;

        if (r[2].second >= r[1].second)
        {
          if (step < 0)
            swap(r[0], r[2]);
          result.reset(r);
          return result;
        }

        r[0] = r[1];
        r[1] = r[2];
        step *= ratio;
      }

      return result;
    }

  private:

    const function<optional<T> (const U&)> _;

    const static size_t max_steps = 100;
    const double ratio;
    const double auto_scale;

  }; // bracketer

  template <typename T, typename U> class brent_minimiser_functor
  // adapted from Numeric Recipes in C (2nd Edition), p. 404
  {

  public:

     brent_minimiser_functor(const function<optional<T> (const U&)>& f)
       : f_(f)
       , bracketer_(f),
      cgold(0.3819660)
    {
    }

    optional<pair<U, T> > operator()(const U& x, const U& scale = U(), const double& tolerance = default_approx_equal_tolerance) const
    {
      const optional<vector<pair<U, T>, 3> > bracket(bracketer_(x, scale));
      if (!bracket)
	return optional<pair<U, T> >();

      U left((*bracket)[0].first), best((*bracket)[1].first), right((*bracket)[2].first);
      U third_best(best), second_best(best);
      T f_best((*bracket)[1].second);
      T f_trial(f_best), f_third_best(f_best), f_second_best(f_best);

      U d(0.), prev_d(0.);

      const double two_tolerance(2. * tolerance);

      for (size_t i(0); i < max_steps; ++i)
      {
        const U mid(0.5 * (left + right));

        if (approx_equal(left, right, two_tolerance))
          return optional<pair<U, T> >(make_pair(best, f_best));

        if (abs(prev_d) > tolerance * abs(best))
        {
          const U r((best - second_best) * (f_best - f_third_best));
          U q((best - third_best) * (f_best - f_second_best));
          U p((best - third_best) * q - (best - second_best) * r);
          q = 2. * (q - r);
          if (q > 0.)
            p = -p;
          q = abs(q);

          U prev_prev_d(prev_d);
          prev_d = d;

          if (abs(p) >= abs(0.5 * q * prev_prev_d) || p <= q * (left - best) || p >= q * (right - best))
            d = cgold * (prev_d = (best >= mid ? left - best : right - best));
          else
            d = p / q;
        }
        else
          d = cgold * (prev_d = best >= mid ? left - best : right - best);

        const U trial(best + d);
        const optional<T> op(f_(trial));
        if (!op)
          return optional<pair<U, T> >();
        f_trial = *op;

        if (f_trial <= f_best)
        {
          if (trial >= best)
            left = best;
          else
            right = best;
          third_best = second_best; second_best = best; best = trial;
          f_third_best = f_second_best; f_second_best = f_best; f_best = f_trial;
        }
        else
        {
          if (trial < best)
            left = trial;
          else
            right = trial;
          if (f_trial <= f_second_best || second_best == best)
          {
            third_best = second_best; second_best = trial;
            f_third_best = f_second_best; f_second_best = f_trial;
          }
          else if (f_trial <= f_third_best || third_best == best || third_best == second_best)
          {
            third_best = trial;
            f_third_best = f_trial;
          }
        }
      }

      return optional<pair<U, T> >();
    }

  private:

    const function<optional<T> (const U&)> f_;
    const bracketer<T, U> bracketer_;

    const static size_t max_steps = 100;
    const double cgold;

  }; // brent_minimiser_functor

  template <class T, class U> brent_minimiser_functor<T, U> brent_minimiser(const function<optional<T> (const U&)>& f)
  {
    return brent_minimiser_functor<T, U>(f);
  }

}
