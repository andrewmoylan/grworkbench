#pragma once

#include <cmath>
#include <lift/vector.hpp>
#include <lift/traits.hpp>
#include "constants.hpp"

namespace grwb
{
  
  using lift::vector;

  template <typename T> inline nvector<T> to_polar(const nvector<T>& x)
  {
    nvector<T> r(x);
    T cosine(abs(x));
    const T zero_t(zero(cosine));
    
    r[0] = cosine;
    for (size_t i(1); i < x.size() - 1; ++i)
      if (cosine != zero_t)
      {        
        r[i] = asin(x[i - 1] / cosine);
        cosine *= cos(r[i]);
      }
      else
        r[i] = zero_t;

    r[x.size() - 1] = atan2(x[x.size() - 2], x[x.size() - 1]);

    return r;
  }

  template <typename T> inline nvector<T> from_polar(const nvector<T>& x)
  {
    nvector<T> r(x);
    T cosine(x[0]);

    for (size_t i(0); i < x.size() - 1; ++i)
    {
      r[i] = cosine * sin(x[i + 1]);
      cosine *= cos(x[i + 1]);
    }
    r[x.size() - 1] = cosine;

    return r;
  }

  template <typename T> inline nvector<T> from_polar_with_radius(const nvector<T>& x, const T& radius)
  {
    nvector<T> x2(x.size() + 1, make_constant_iterator(radius));
    for (size_t i(0); i < x.size(); ++i)
      x2[i + 1] = x[i];
    return from_polar(x2);
  }

  template <typename T> inline nvector<T> to_polar_without_radius(const nvector<T>& x)
  {
    const nvector<T> polar(to_polar(x));
    return nvector<T>(polar.size() - 1, polar.begin() + 1);
  }
}

