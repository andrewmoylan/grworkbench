#pragma once

#include <cmath>
#include <lift/differential.hpp>
#include <lift/elementary.hpp>
#include "constants.hpp"
#include "nvector.hpp"
#include "atlas.hpp"
#include "opengl.hpp"
#include "nvector_differential.hpp"

namespace grwb
{
  using std::sqrt;
  using lift::sqr;

  namespace kerr_newman
  {
    class boyer_lindquist
    {
    public:
      boyer_lindquist(const double&, const double&, const double&, const int);
      optional<nvector<nvector<differential<double, nvector<double> > > > > operator()(const nvector<differential<double, nvector<double> > >&) const;

    private:
      const double mass;
      const double angular_momentum;
			const double charge;

			const int region;

			const double r_plus;
			const double r_minus;

			boyer_lindquist& operator=(const boyer_lindquist&);
    };

    class revolve
    {
    public:
      optional<nvector<differential<double, nvector<double> > > > operator()(const nvector<differential<double, nvector<double> > >&) const;
    };

		class kerr_coordinates
		{
		public:
			kerr_coordinates(const double&, const double&, const double&, const bool);
			optional<nvector<nvector<differential<double, nvector<double> > > > > operator()(const nvector<differential<double, nvector<double> > >&) const;

		private:
			const double m;
			const double a;
			const double e;

			const bool ingoing;

			kerr_coordinates& operator=(const kerr_coordinates&);
		};

		class boyer_lindquist_to_kerr_coordinates
		{
		public:
			boyer_lindquist_to_kerr_coordinates(const double&, const double&, const double&, const int, const bool, const bool);
      optional<nvector<differential<double, nvector<double> > > > operator()(const nvector<differential<double, nvector<double> > >&) const;

		private:
			const double m;
			const double a;
			const double e;

			const int region;
			const bool is_inverse;
			const bool to_ingoing;

			const double r_plus;
			const double r_minus;
			const double k_plus;
			const double k_minus;

			const differential<double, nvector<double> > phi_integral(const differential<double, nvector<double> >&) const;
			const differential<double, nvector<double> > t_integral(const differential<double, nvector<double> >&) const;

			boyer_lindquist_to_kerr_coordinates& operator=(const boyer_lindquist_to_kerr_coordinates&);
		};
  }
}
