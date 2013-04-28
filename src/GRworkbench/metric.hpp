#pragma once

#include "tensor.hpp"

namespace grwb
{

  inline double metric(const std::shared_ptr<chart>& c, const std::shared_ptr<point> & a, const nvector<double>& v1, const nvector<double>& v2)
  {
    nvector<differential<double, nvector<double> > > i(make_gradient(*(*a)[c]));
    optional<nvector<nvector<differential<double, nvector<double> > > > > j((*c)(i));
    nvector<nvector<differential<double, nvector<double> > > > k(*j);
    nvector<nvector<double> > l(value(k));
    return inner_product(inner_product(l, v1), v2);
  }

  inline double metric(const tangent_vector& v1, const tangent_vector& v2)
  {
    const std::shared_ptr<point> & a(v1.context());
    const std::shared_ptr<chart>& c(a->valid_chart());
    return metric(c, a, *v1[c], *v2[c]);
  }

  inline double metric(const tangent_vector& v)
  {
    return metric(v, v);
  }
  
  inline double metric(const std::shared_ptr<chart>& c, const std::shared_ptr<point> & a, const nvector<double>& v)
  {
    return metric(c, a, v, v);
  }
}
