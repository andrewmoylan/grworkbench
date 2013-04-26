#pragma once

#include "tensor.hpp"
#include "worldline.hpp"
#include "boost/bind.hpp"

namespace grwb
{
  class geodesic
  {
  public:
    geodesic(const tangent_vector& t);    
    optional<shared_ptr<point> > value(const double& t) const;
    pair<double, optional<shared_ptr<point> > > operator()(const double& t, const double& epsilon) const;    
    optional<shared_ptr<tangent_vector> > operator()(const double& t) const;

		const worldline as_worldline() const
		{
			return bind(&geodesic::value, *this, _1);
		}

  private:
    mutable double least_upper_bound_;
    mutable double greatest_lower_bound_;
    shared_ptr<std::map<double, shared_ptr<tangent_vector> > > cache_;
    typedef std::map<double, shared_ptr<tangent_vector> >::const_iterator cache_iterator_type;
    typedef std::map<double, shared_ptr<tangent_vector> >::value_type cache_value_type;
    class geodesic_callback
    {
    public:
      geodesic_callback(const shared_ptr<chart>& c);
      optional<vector<nvector<double>, 2> > operator()(const double& t, const vector<nvector<double>, 2>& y) const;
    private:
      const shared_ptr<chart> _;
      geodesic_callback& operator=(const geodesic_callback&);
    };
    cache_iterator_type get_initial_data(const double& t) const;
    optional<cache_iterator_type> advance(const tangent_vector& tangent, const double& from_t, const double& to_t, size_t recursion = 1) const;
    optional<cache_iterator_type> advance_on_chart(const shared_ptr<chart>& c, const nvector<double>& x, const nvector<double>& dx, const double& from_t, const double& to_t) const;
    const static size_t max_recursion_;
  };
}
