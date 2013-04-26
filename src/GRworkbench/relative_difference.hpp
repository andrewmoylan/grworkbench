#pragma once

#include <lift/elementary.hpp>
#include "nvector.hpp"

namespace grwb
{
  
  using std::abs;

  template <typename T> struct relative_difference_implementation
  {
    static double apply(const T& a, const T& b)
    {
      const double abs_a_abs_b(abs(a) * abs(b));
      const double abs_a_minus_b(abs(a - b));
      return abs_a_abs_b <= 1 ? abs_a_minus_b : abs_a_minus_b / sqrt(abs_a_abs_b);
    }

    static double apply_squared(const T& a, const T& b)
    {
      const double abs_a_abs_b(abs(a) * abs(b));
      const double abs_a_minus_b_squared(sqr(abs(a - b)));
      return abs_a_abs_b <= 1 ? abs_a_minus_b_squared : abs_a_minus_b_squared / abs_a_abs_b;
    }
  };

  template <typename T> double relative_difference(const T& a, const T& b)
  {
    return relative_difference_implementation<T>::apply(a, b);
  }

  template <typename T> double relative_difference_squared(const T& a, const T& b)
  {
    return relative_difference_implementation<T>::apply_squared(a, b);
  }

  template <typename T> struct relative_difference_implementation<nvector<T> >
  {
    static double apply(const nvector<T>& a, const nvector<T>& b)
    {
      return sqrt(apply_squared(a, b));
    }

    static double apply_squared(const nvector<T>& a, const nvector<T>& b)
    {
      same_size(a, b);
      
      typename nvector<T>::const_iterator i, j;
      double r(0.);
      for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
	r += relative_difference_squared(*i, *j);

      return r;
    }
  };

  using lift::vector;

  template <typename T, size_t N> struct relative_difference_implementation<vector<T, N> >
  {
    static double apply(const vector<T, N>& a, const vector<T, N>& b)
    {
      return sqrt(apply_squared(a, b));
    }

    static double apply_squared(const vector<T, N>& a, const vector<T, N>& b)
    {
      typename vector<T, N>::const_iterator i, j;
      double r(0.);
      for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
	r += relative_difference_squared(*i, *j);

      return r;
    }
  };

}
