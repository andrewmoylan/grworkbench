#pragma once

#include "tensor.hpp"
#include "worldline.hpp"


namespace grwb
{
  class geodesic
  {
  public:
    geodesic(const tangent_vector& t);    
    optional<std::shared_ptr<point> > value(const double& t) const;
    pair<double, optional<std::shared_ptr<point> > > operator()(const double& t, const double& epsilon) const;    
    optional<std::shared_ptr<tangent_vector> > operator()(const double& t) const;

		const worldline as_worldline() const
		{
			return std::bind(&geodesic::value, *this, std::placeholders::_1);
		}

  private:
    mutable double least_upper_bound_;
    mutable double greatest_lower_bound_;
    std::shared_ptr<std::map<double, std::shared_ptr<tangent_vector> > > cache_;
    typedef std::map<double, std::shared_ptr<tangent_vector> >::const_iterator cache_iterator_type;
    typedef std::map<double, std::shared_ptr<tangent_vector> >::value_type cache_value_type;
    class geodesic_callback
    {
    public:
      geodesic_callback(const std::shared_ptr<chart>& c);
      optional<vector<nvector<double>, 2> > operator()(const double& t, const vector<nvector<double>, 2>& y) const;
    private:
      const std::shared_ptr<chart> _;
      geodesic_callback& operator=(const geodesic_callback&);
    };
    cache_iterator_type get_initial_data(const double& t) const;
    optional<cache_iterator_type> advance(const tangent_vector& tangent, const double& from_t, const double& to_t, size_t recursion = 1) const;
    optional<cache_iterator_type> advance_on_chart(const std::shared_ptr<chart>& c, const nvector<double>& x, const nvector<double>& dx, const double& from_t, const double& to_t) const;
    const static size_t max_recursion_;
  };
}
