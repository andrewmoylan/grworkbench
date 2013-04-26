#include "kerr.hpp"
#include "tensor.hpp"

namespace grwb
{
	namespace kerr_newman
	{
		using lift::make_vector;

		boyer_lindquist::boyer_lindquist(const double& m, const double& a, const double& e, const int r)
			: mass(m),
			angular_momentum(a),
			charge(e),
			r_plus(m + sqrt(sqr(m) - sqr(a) - sqr(e))),
			r_minus(m - sqrt(sqr(m) - sqr(a) - sqr(e))),
			region(r)
		{
		}

		using lift::differential;
		using lift::value;
		using lift::gradient;

		optional<nvector<nvector<differential<double, nvector<double> > > > > boyer_lindquist::operator()(const nvector<differential<double, nvector<double> > >& x) const
		{
			const double a_2(sqr(angular_momentum));
			const double e_2(sqr(charge));

			differential<double, nvector<double> > delta((x[1] - (2. * mass)) * x[1] + a_2 + e_2);

			const bool in_region(
				((sqr(mass) < a_2 + e_2 && region == 1) ||
				(x[1] > r_plus && region == 1
				|| x[1] < r_minus && region == 3
				|| region == 2 && x[1] > r_minus && x[1] < r_plus))
				&& (x[1] > 0. || a_2 != 0.));

			if (!in_region || x[2] <= 0. || x[2] >= pi || x[3] <= 0. || x[3] >= 2. * pi)
				return optional<nvector<nvector<differential<double, nvector<double> > > > >();

			nvector<nvector<differential<double, nvector<double> > > > result(4, make_constant_iterator(nvector<differential<double, nvector<double> > >(4, make_constant_iterator(differential<double, nvector<double> >(0, nvector<double>(4, make_constant_iterator(0)))))));

			const differential<double, nvector<double> > r_2(sqr(x[1]));
			const differential<double, nvector<double> > sin_2_theta(sqr(sin(x[2])));
			const differential<double, nvector<double> > cos_2_theta(sqr(cos(x[2])));
			const differential<double, nvector<double> > rho_2(r_2 + a_2 * cos_2_theta);

			result[0][0] = (a_2 * sin_2_theta - delta) / rho_2;
			result[0][3] = (e_2 - 2. * mass * x[1]) * angular_momentum * sin_2_theta / rho_2;
			result[1][1] = rho_2 / delta;
			result[2][2] = rho_2;
			result[3][0] = result[0][3];
			result[3][3] = (sqr(r_2 + a_2) - (a_2 * delta * sin_2_theta)) * sin_2_theta / rho_2;

			return result;
		}

		optional<nvector<differential<double, nvector<double> > > > revolve::operator()(const nvector<differential<double, nvector<double> > >& x) const
		{
			if (x[3] == pi)
				return optional<nvector<differential<double, nvector<double> > > >();
			nvector<differential<double, nvector<double> > > result(x);
			result[3] += (x[3] < pi) ? pi : -pi;
			return result;
		}

		kerr_coordinates::kerr_coordinates(const double& m_, const double& a_, const double& e_, const bool ingo)
			: m(m_),
			a(a_),
			e(e_),
			ingoing(ingo)
		{
		}

		optional<nvector<nvector<differential<double, nvector<double> > > > > kerr_coordinates::operator ()(const nvector<differential<double, nvector<double> > >& x) const
		{
			if ((x[1] <= 0. && a == 0.) || x[2] <= 0. || x[2] >= pi || x[3] <= 0. || x[3] >= 2. * pi)
				return optional<nvector<nvector<differential<double, nvector<double> > > > >();

			nvector<nvector<differential<double, nvector<double> > > > result(4, make_constant_iterator(nvector<differential<double, nvector<double> > >(4, make_constant_iterator(differential<double, nvector<double> >(0, nvector<double>(4, make_constant_iterator(0)))))));

			const double a_2(sqr(a));
			const double e_2(sqr(e));
			const differential<double, nvector<double> > delta((x[1] - (2. * m)) * x[1] + a_2 + e_2);
			const differential<double, nvector<double> > r_2(sqr(x[1]));
			const differential<double, nvector<double> > sin_2_theta(sqr(sin(x[2])));
			const differential<double, nvector<double> > cos_2_theta(sqr(cos(x[2])));
			const differential<double, nvector<double> > rho_2(r_2 + a_2 * cos_2_theta);
			const double sign(ingoing ? 1. : -1.);

			result[0][0] = (2. * m * x[1] - e_2) / rho_2 - 1.;
			result[0][3] = (e_2 - 2. * m * x[1]) * a * sin_2_theta / rho_2;
			result[3][0] = result[0][3];
			result[0][1] = sign * differential<double, nvector<double> >(1., nvector<double>(gradient(x).size()));
			result[1][0] = result[0][1];
			result[1][3] = sign * (- a * sin_2_theta);
			result[3][1] = result[1][3];
			result[2][2] = rho_2;
			result[3][3] = (sqr(r_2 + a_2) - (a_2 * delta * sin_2_theta)) * sin_2_theta / rho_2;

			return result;
		}

		boyer_lindquist_to_kerr_coordinates::boyer_lindquist_to_kerr_coordinates(const double& m_, const double& a_, const double& e_, const int r_, const bool inv_, const bool ingo_)
			: m(m_),
			a(a_),
			e(e_),
			region(r_),
			is_inverse(inv_),
			r_plus(m_ + sqrt(sqr(m_) - sqr(a_) - sqr(e_))),
			r_minus(m_ - sqrt(sqr(m_) - sqr(a_) - sqr(e_))),
			// from Barret O'Neil, The geometry of Kerr black holes, p. 122
			k_plus((r_plus - r_minus) / 2. / (sqr(r_plus) + sqr(a))),
			k_minus((r_minus - r_plus) / 2. / (sqr(r_minus) + sqr(a))),
			to_ingoing(ingo_)
		{
		}

		const differential<double, nvector<double> > boyer_lindquist_to_kerr_coordinates::phi_integral(const differential<double, nvector<double> >& r) const
		{
			const differential<double, nvector<double> > A(region == 1 ? 3. * m : (region == 2 ? m : r_minus / 2.), nvector<double>(gradient(r).size()));
			if (sqr(a) + sqr(e) == sqr(m))
			{
				// see Andrew Moylan, "Kerr coordinates.nb" (Mathematica notebook)
				return a * (1. / (A - m) + 1. / (m - r));
			}
			else
			{
				// generalised to e != 0 from Barret O'Neil, The geometry of Kerr black holes, p. 122
				return a / (r_plus - r_minus) * log(abs((r - r_plus) / (r - r_minus)));
			}
		}

		const differential<double, nvector<double> > boyer_lindquist_to_kerr_coordinates::t_integral(const differential<double, nvector<double> >& r) const
		{
			const differential<double, nvector<double> > A(region == 1 ? 3. * m : (region == 2 ? m : r_minus / 2.), nvector<double>(gradient(r).size()));
			if (sqr(a) + sqr(e) == sqr(m))
			{
				// see Andrew Moylan, "Kerr coordinates.nb" (Mathematica notebook)
				return ((A - r) * (sqr(a) + m * (2. * m - r) + A * (r - m)) + 2. * (A - m) * m * (m - r) * log(abs((r - m) / (A - m)))) / (A - m) / (m - r);
			}
			else
			{
				// generalised to e != 0 from Barret O'Neil, The geometry of Kerr black holes, p. 122
				return r + 0.5 / k_plus * log(abs(r - r_plus)) + 0.5 / k_minus * log(abs(r - r_minus));
			}
		}

		optional<nvector<differential<double, nvector<double> > > > boyer_lindquist_to_kerr_coordinates::operator ()(const nvector<differential<double, nvector<double> > >& x) const
		{
			const bool in_region(
				((sqr(m) < sqr(a) + sqr(e) && region == 1) ||
				(x[1] > r_plus && region == 1
				|| x[1] < r_minus && region == 3
				|| region == 2 && x[1] > r_minus && x[1] < r_plus))
				&& (x[1] > 0. || a != 0.));

			if (!in_region)
				return optional<nvector<differential<double, nvector<double> > > >();

			nvector<differential<double, nvector<double> > > result(x);

			const double sign((to_ingoing == !is_inverse) ? 1. : -1.);

			result[0] += sign * t_integral(x[1]);
			result[3] += sign * phi_integral(x[1]);

			value(result[3]) = fmod(value(result[3]), two_pi);
			if (result[3] < 0.)
				result[3] += two_pi;
			else if (!(result[3] > 0. && result[3] < two_pi))
				return optional<nvector<differential<double, nvector<double> > > >();

			return result;
		}
	}
}
