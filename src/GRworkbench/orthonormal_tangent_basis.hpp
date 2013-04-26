#pragma once

#include "matrix.hpp"
#include "point.hpp"
#include "metric.hpp"

namespace grwb
{
  using std::swap;

  inline nvector<nvector<double> > orthonormal_tangent_basis(const shared_ptr<point> & p, const shared_ptr<chart>& c)
  {
    nvector<nvector<double> > r((eigen<double>(value(*(*c)(make_gradient(*(*p)[c]))))).vectors);
    for (size_t i(0); i < r.size(); ++i)
    {
      const double m(metric(c, p, r[i]));
      r[i] /= sqrt(abs(m));
      if (m < 0)
        swap(r[0], r[i]);
    }
    return transpose(r);
  }
}
