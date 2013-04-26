#pragma once

#include <lift/elementary.hpp>
#include "relative_difference.hpp"

namespace grwb
{  
  const double default_approx_equal_tolerance = 1.e-6;

  using lift::sqr;

  template <typename T> bool approx_equal(const T& a, const T& b, const double& tolerance = default_approx_equal_tolerance)
  {
    return relative_difference_squared(a, b) < sqr(tolerance);
  }
}
