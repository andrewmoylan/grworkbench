#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <lift/constant_iterator.hpp>
#include "matrix.hpp"
#include "powell_minimiser.hpp"
#include "constants.hpp"
#include "euclidean_separation.hpp"
#include "geodesic.hpp"
#include "orthonormal_tangent_basis.hpp"
#include "point.hpp"
#include "polar_transform.hpp"

namespace grwb
{
	namespace connecting_null_geodesic_detail
	{
		class null_geodesic_shooter
		{
		public:
			null_geodesic_shooter(const function<optional<std::shared_ptr<point> > (const double&)>& curve, const std::shared_ptr<point> & a, const std::shared_ptr<chart>& c, const nvector<nvector<double> >& tangent_basis, const double& null_guess, const double& guess)
				: guess_(make_vector(null_guess, guess)),
				curve_(curve),
				a_(a),
				chart_(c),          
				basis(tangent_basis)
			{
			}

			optional<double> operator()(const nvector<double>& v) const
			{
				const nvector<double> v2(from_polar_with_radius(v, 1.));
				nvector<double> v3(v2.size() + 1, make_constant_iterator(1.));
				for (size_t i(0); i < v2.size(); ++i)
					v3[i + 1] = v2[i];

				geodesic geo(tangent_vector(a_, chart_, basis * v3));
				optional<pair<nvector<double>, double> > r(min_euclidean_separation(geo.as_worldline(), curve_, guess_));

				if (!r)
					return optional<double>();

				guess_ = r->first;
				//                        log_cout << v << guess_ << (basis * v3) << r->second << endl;
				return optional<double>(r->second);
			}

			const nvector<double>& last_guess() const
			{
				return guess_;
			}

		private:
			mutable nvector<double> guess_;
			const function<optional<std::shared_ptr<point> > (const double&)>& curve_;
			const std::shared_ptr<point> & a_;
			const std::shared_ptr<chart>& chart_;
			const nvector<nvector<double> >& basis;
		};
	}

	using std::make_pair;

	inline optional<pair<double, std::shared_ptr<tangent_flow> > > connecting_null_geodesic(const worldline& curve, const std::shared_ptr<point> & a, const double& guess)
	{
		const static nvector<nvector<double> > polar_basis(make_vector(make_vector(0.01, 0.), make_vector(0., 0.01)));

		const std::shared_ptr<chart>& c(a->valid_chart());
		const nvector<nvector<double> > basis(orthonormal_tangent_basis(a, c));

		const std::shared_ptr<point>  b(*curve(guess));

		const nvector<double> va(*(*a)[c]), vb(*(*b)[c]);
		const nvector<double> spacelike(vb.size() - 1, (inverse(basis) * (vb - va)).begin() + 1);
		const nvector<double> spacelike_polar(to_polar_without_radius(spacelike));

		const connecting_null_geodesic_detail::null_geodesic_shooter shooter(curve, a, c, basis, abs(spacelike), guess);
		const function<optional<double> (const nvector<double>&)> shooter_function(shooter);
		const powell_minimiser<double, nvector<double> > pm(shooter_function);

		optional<pair<nvector<double>, double> > r(pm(spacelike_polar, polar_basis));
		if (!r)
			return optional<pair<double, std::shared_ptr<tangent_flow> > >();

		const nvector<double> vr(from_polar_with_radius(r->first, 1.));
		nvector<double> vr2(vr.size() + 1, make_constant_iterator(1.));
		for (size_t i(0); i < vr.size(); ++i)
			vr2[i + 1] = vr[i];
		const nvector<double> solution(basis * vr2);   

		const nvector<double> scales(min_euclidean_separation(geodesic(tangent_vector(a, c, solution)).as_worldline(), curve, shooter.last_guess())->first);

		return make_pair(scales[1], std::shared_ptr<tangent_flow>(new tangent_flow(geodesic(tangent_vector(a, c, solution * scales[0])))));
	}

	inline optional<pair<double, std::shared_ptr<tangent_flow> > > connecting_null_geodesic(const std::shared_ptr<point>& a, const std::shared_ptr<worldline>& curve, const double& guess)
	{
		return connecting_null_geodesic(*curve, a, guess);
	}
}
