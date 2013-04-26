#pragma once

#include <cmath>
#include <lift/vector.hpp>
#include <lift/traits.hpp>
#include "nvector.hpp"
#include "approx_equal.hpp"

namespace lift
{
  template <class T> inline grwb::nvector<grwb::nvector<T> > unity(const grwb::nvector<grwb::nvector<T> >& prototype)
  {
    grwb::nvector<grwb::nvector<T> > r(prototype);
    for (size_t i(0); i < r.size(); ++i)
      for (size_t j(0); j < r.size(); ++j)
        r[i][j] = i == j ? unity(r[i][j]) : zero(r[i][j]);
    return r;
  }
}

namespace grwb
{

  template <typename T> nvector<nvector<T> > transpose(const nvector<nvector<T> >& that)
  {
    const size_t m(that.size()), n(that[0].size());
    nvector<nvector<T> > result(n, make_constant_iterator(nvector<T>(m, make_constant_iterator(that[0][0]))));
    for (size_t i(0); i < m; ++i)
      for (size_t j(0); j < n; ++j)
        result[j][i] = that[i][j];
    return result;
  }

  template<typename T> class eigen
  {
  public:
    eigen(const nvector<nvector<T> >&);
    nvector<nvector<T> > vectors;
    nvector<T> values;	
  };
  
  template<typename T> nvector<T> operator*(const nvector<nvector<T> >& left, const nvector<T>& right)
  {
    nvector<T> result(left.size());
    for (size_t i = 0; i < left.size(); ++i)
      result[i] = inner_product(left[i], right);
    return result;
  }
}

namespace lift
{
  template <typename T> grwb::nvector<T> zero(const grwb::nvector<T>& a)
  {
    return grwb::nvector<T>(a.size(), make_constant_iterator(zero(a[0])));
  }
}
 
namespace grwb
{

  using lift::zero;
  using lift::unity;

  template <typename T> eigen<T>::eigen(const nvector<nvector<T> >& that)
    : vectors(zero(that)),
      values(zero(that[0]))
  {
    const size_t n(that.size());
    for (size_t i(0); i < n; ++i)
    {
      for (size_t j(0); abs(vectors[i]) < 0.1; ++j)
      {
        vectors[i] = zero(vectors[i]);
        vectors[i][j] = unity(vectors[i][0]);
        for (size_t k(0); k < i; ++k)
          vectors[i] -= vectors[k] * inner_product(vectors[k], vectors[i]);
      }

      nvector<T> old_vector(that.size());
      do
      {
        values[i] = inner_product(vectors[i], old_vector);
        vectors[i] /= abs(vectors[i]);
	old_vector = vectors[i];
        vectors[i] = that * vectors[i]; // transform
        for (size_t k = 0; k < i; ++k)
          vectors[i] -= vectors[k] * inner_product(vectors[k], vectors[i]); // render linearly independant
      }
      while (std::abs(abs(vectors[i]) - fabs(values[i])) > default_approx_equal_tolerance); // see if stabilised
      vectors[i] /= abs(vectors[i]);
    }
  }

  class singular
    : public std::exception
  {
  };

  template<typename T> nvector<nvector<T> > inverse(const nvector<nvector<T> >& that)
  {
    const size_t n(that.size());
    nvector<nvector<T> > left(that), right(that);

    // set right = identity
    for (size_t i = 0; i < n; ++i)
      for (size_t j = 0; j < n; ++j)
        right[i][j] = i == j ? 1. : 0.;

    for (size_t i = 0; i < n; ++i)
    {
      // move best pivot row up to [i]
      for (size_t j = i + 1; j < n; ++j)
        if (abs(left[j][i]) > abs(left[i][i]))
        {
          left[i].swap(left[j]);
          right[i].swap(right[j]);
        }

      if (!left[i][i])
			{
        throw singular();
			}

      T normalise(left[i][i]);
      right[i] /= normalise;
      left[i] /= normalise;

      // row reduction
      for (size_t j = 0; j < n; ++j)
        if (j != i)
        {
          T scale(left[j][i]);
          right[j] -= right[i] * scale;
          left[j] -= left[i] * scale;
        }
    }

    return right;
  }
}
