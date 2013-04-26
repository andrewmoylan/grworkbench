#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <lift/vector.hpp>
#include "powell_minimiser.hpp"
#include "point.hpp"
#include "orthonormal_tangent_basis.hpp"
#include "worldline.hpp"

namespace grwb
{
	using namespace lift;

  inline optional<double> euclidean_separation(const shared_ptr<point> & a, const shared_ptr<point> & b)
  {
    optional<double> r;

    for (set<shared_ptr<chart> >::const_iterator i(charts().begin()); i != charts().end(); ++i)      
    {
      const optional<nvector<double> > xa((*a)[*i]), xb((*b)[*i]);
      if (xa && xb)
      {
        const double d(abs(inverse(orthonormal_tangent_basis(a, *i)) * (*xa - *xb)));
        if (!r || *r > d)
          r.reset(d);
      }	
    }

    return r;
  }

  namespace euclidean_separation_detail
  {
    class curve_point_separation
    {
    public:
      curve_point_separation(const worldline& curve, const shared_ptr<point> & a)
        : curve_(curve),
          a_(a)
      {
      }
      
      optional<double> operator()(const double& t) const
      {
        optional<shared_ptr<point> > b(curve_(t));
        return b ? euclidean_separation(a_, *b) : optional<double>();
      }
      
    private:
      const worldline& curve_;
      const shared_ptr<point> & a_;
    };

    class curve_curve_separation
    {
    public:
      curve_curve_separation(const function<optional<shared_ptr<point> > (const double&)>& curve_a, const function<optional<shared_ptr<point> > (const double&)>& curve_b)
        : curve_a_(curve_a),
          curve_b_(curve_b)
      {
      }
      
      optional<double> operator()(const nvector<double>& t) const
      {
        optional<shared_ptr<point> > a(curve_a_(t[0]));
        optional<shared_ptr<point> > b(curve_b_(t[1]));
        return a && b ? euclidean_separation(*a, *b) : optional<double>();
      }
      
    private:
      const function<optional<shared_ptr<point> > (const double&)>& curve_a_;
      const function<optional<shared_ptr<point> > (const double&)>& curve_b_;
    };
  }

  inline optional<pair<double, double> > min_euclidean_separation(const function<optional<shared_ptr<point> > (const double&)>& curve, const shared_ptr<point> & a)
  {
    return brent_minimiser_functor<double, double>(euclidean_separation_detail::curve_point_separation(curve, a))(0.);
  }

  inline optional<pair<double, double> > min_euclidean_separation(const shared_ptr<point> & a, const function<optional<shared_ptr<point> > (const double&)>& curve)
  {
    return min_euclidean_separation(curve, a);
  }

  inline optional<pair<nvector<double>, double> > min_euclidean_separation(const function<optional<shared_ptr<point> > (const double&)>& curve_a, const function<optional<shared_ptr<point> > (const double&)>& curve_b, const nvector<double>& guess)  
  {
    static const double auto_scale(1.e-2);
    const double scale_a(guess[0] != 0. ? auto_scale * guess[0] : 1.);
    const double scale_b(guess[1] != 0. ? auto_scale * guess[1] : 1.);
    return powell_minimiser<double, nvector<double> >(euclidean_separation_detail::curve_curve_separation(curve_a, curve_b))(guess, make_vector(make_vector(scale_a, scale_b), make_vector(scale_a, - scale_b)));
  }
  
  inline optional<pair<nvector<double>, double> > min_euclidean_separation(const function<optional<shared_ptr<point> > (const double&)>& curve_a, const function<optional<shared_ptr<point> > (const double&)>& curve_b)
  {
    return min_euclidean_separation(curve_a, curve_b, make_vector(0., 0.));
  }
}
