#pragma once

#include <string>
#include "constants.hpp"
#include "distortion.hpp"
#include <lift/constant_iterator.hpp>

namespace grwb
{  
  using lift::make_constant_iterator;

  class spherical_orthonormal : public distortion
  {
  public:
    virtual optional<nvector<differential<double, nvector<double> > > > operator()(const nvector<differential<double, nvector<double> > >& p) const 
    {
      if (p.size() != 4 || p[1] <= 0.)
        return optional<nvector<differential<double, nvector<double> > > >();

      nvector<differential<double, nvector<double> > > r(p.size(), make_constant_iterator(differential<double, nvector<double> >(0., nvector<double>(gradient(p[0]).size(), make_constant_iterator(0.)))));

      const differential<double, nvector<double> > sine_theta(sin(p[2]));
      r[0] = p[0];
      r[1] = p[1] * sine_theta * cos(p[3]);
      r[2] = p[1] * sine_theta * sin(p[3]);
      r[3] = p[1] * cos(p[2]);

      return r;
		}
	};
}
