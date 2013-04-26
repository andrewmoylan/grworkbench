#pragma once

#include <lift/vector.hpp>

namespace grwb
{
  using lift::vector;

  template <typename U> bool colinear(const vector<U, 3>& a, const vector<U, 3>& b, const vector<U, 3>& c, const double& cosine_threshold = 0.9975)
  {
    vector<U, 3> p(b - a);
    vector<U, 3> q(c - b);

    return cosine_threshold < inner_product(p, q) / (abs(p) * abs(q));
  }
}
