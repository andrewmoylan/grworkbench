#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include "point.hpp"
#include "nvector.hpp"

namespace grwb
{
  using boost::function;
  using boost::optional;
  using lift::vector;

  typedef function<optional<shared_ptr<point> > (const double&)> worldline;
	typedef function<optional<nvector<double> > (const double&)> coordinate_line;
  typedef function<optional<nvector<double> >(const vector<double, 2>&)> coordinate_surface;
	typedef function<optional<shared_ptr<point> >(const vector<double, 2>&)> spacetime_surface;
	typedef function<optional<shared_ptr<tangent_vector> > (const double&)> tangent_flow;
}
