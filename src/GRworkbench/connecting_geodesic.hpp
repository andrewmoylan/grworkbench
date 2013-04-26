#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include "powell_minimiser.hpp"
#include "polar_transform.hpp"
#include "geodesic.hpp"
#include "point.hpp"
#include "tensor.hpp"
#include "euclidean_separation.hpp"

namespace grwb
{

  namespace connecting_geodesic_detail
  {
    class geodesic_shooter
    {
    public:
      geodesic_shooter(const shared_ptr<point> & a, const shared_ptr<point> & b, const shared_ptr<chart>& c)
        : a_(a),
          b_(b),
          chart_(c)
      {
      }
      
      optional<double> operator()(const nvector<double>& v) const
      {
        geodesic geo(tangent_vector(a_, chart_, from_polar_with_radius(v, 1.)));
        optional<pair<double, double> > r(min_euclidean_separation(geo.as_worldline(), b_));
        return r ? optional<double>(r->second) : optional<double>();
      }
      
    private:
      const shared_ptr<point> & a_;
      const shared_ptr<point> & b_;
      const shared_ptr<chart>& chart_;
    };
  }

  inline optional<shared_ptr<tangent_flow> > connecting_geodesic(const shared_ptr<point> & a, const shared_ptr<point> & b)
  {
    const shared_ptr<chart>& c(a->valid_chart());
    connecting_geodesic_detail::geodesic_shooter shooter(a, b, c);

    function<optional<double> (const nvector<double>&)> shooter_function(shooter);
    powell_minimiser<double, nvector<double> > pm(shooter_function);
 
    const nvector<double> va(*(*a)[c]), vb(*(*b)[c]);
    optional<pair<nvector<double>, double> > r(pm(to_polar_without_radius(vb - va)));

    if (!r)
      return optional<shared_ptr<tangent_flow> >();

    const nvector<double> v(from_polar_with_radius(r->first, 1.));
    const double scale(min_euclidean_separation(geodesic(tangent_vector(a, c, v)).as_worldline(), b)->first);

    return shared_ptr<tangent_flow>(new tangent_flow(geodesic(tangent_vector(a, c, v * scale))));
  }
}
