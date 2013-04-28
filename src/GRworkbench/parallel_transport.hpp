#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/weak_ptr.hpp>
#include "bulirsch_stoer.hpp"
#include "modified_midpoint_stepper.hpp"
#include "connection.hpp"
#include "point.hpp"
#include "tensor.hpp"
#include "worldline.hpp"

namespace grwb
{
  class parallel_transport
  {
  public:
    parallel_transport(const tangent_flow& curve, const std::shared_ptr<tangent_vector>& t)
      : cache_(new std::map<double, std::shared_ptr<tangent_vector> >),
        curve_(curve)
    {
      cache_->insert(cache_value_type(0, t));
    }
    
    optional<std::shared_ptr<tangent_vector> > operator()(const double& x) const
    {
      return operator()(x, 0).second;
    }
    
    pair<double, optional<std::shared_ptr<tangent_vector> > > operator()(const double& x, const double& epsilon) const
    {
      typedef pair<double, optional<std::shared_ptr<tangent_vector> > > return_type;
      
      if (!curve_(x))
        return return_type(x, optional<std::shared_ptr<tangent_vector> >());
      
      cache_iterator_type initial_data(get_initial_data(x));
      
      if (abs(initial_data->first - x) <= epsilon)
        return return_type(initial_data->first, optional<std::shared_ptr<tangent_vector> >(initial_data->second));
      
      optional<cache_iterator_type> result(advance(*initial_data->second, initial_data->first, x));
      
      if (result)
        return return_type(x, optional<std::shared_ptr<tangent_vector> >((*result)->second));
      
      return return_type(x, optional<std::shared_ptr<tangent_vector> >());
    }
    
  private:
    std::shared_ptr<std::map<double, std::shared_ptr<tangent_vector> > > cache_;
    const tangent_flow curve_;

    typedef std::map<double, std::shared_ptr<tangent_vector> >::const_iterator cache_iterator_type;
    typedef std::map<double, std::shared_ptr<tangent_vector> >::value_type cache_value_type;


    cache_iterator_type get_initial_data(const double& x) const
    {
      cache_iterator_type after(cache_->lower_bound(x));
      
      if (after == cache_->begin())
        return after;
      
      if (after == cache_->end())
        return --cache_iterator_type(after);
      
      cache_iterator_type before(after);
      --before;
      return abs(before->first - x) < abs(after->first - x) ? before : after;
    }
    
    optional<cache_iterator_type> advance(const tangent_vector& tangent, const double& from_t, const double& to_t, size_t recursion = 1) const
    {
      if (recursion > max_recursion_)
        return optional<cache_iterator_type>();
      
      //log_cout << "  Parallel transport: " << from_t << " -> " << to_t << endl;
      
      for (set<std::shared_ptr<chart> >::const_iterator i = charts().begin(); i != charts().end(); ++i)
        if (tangent[*i])
        {
          optional<cache_iterator_type> result(advance_on_chart(*i, *tangent[*i], from_t, to_t));
          if (result)
            return result;
        }
      
      optional<cache_iterator_type> halfway(advance(tangent, from_t, (from_t + to_t) / 2, recursion + 1));
      return halfway ? advance(*(*halfway)->second, (from_t + to_t) / 2, to_t, recursion + 1) : optional<cache_iterator_type>();
    }
    
    class parallel_transport_callback
    {
    public:
      parallel_transport_callback(const std::shared_ptr<chart>& c, const tangent_flow& cv)
        : chart_(c),
          curve_(cv)
      {
      }
      
      optional<nvector<double> > operator()(const double& t, const nvector<double>& y) const
      {
        const size_t dim(y.size());
        nvector<double> dy(dim);
        
        optional<std::shared_ptr<tangent_vector> > tangent(curve_(t));
        if (!tangent || !(**tangent)[chart_])
          return optional<nvector<double> >();
        
        nvector<double> vtan(*(**tangent)[chart_]);
        optional<nvector<nvector<nvector<double> > > > con((connection(*chart_))(*(*(*tangent)->context())[chart_]));
        if (!con)
          return optional<nvector<double> >();
        
        for (size_t a = 0; a < dim; ++a)
          for (size_t b = 0; b < dim; ++b)
            for (size_t c = 0; c < dim; ++c)
              dy[a] -= vtan[b] * (*con)[a][b][c] * y[c];
        
        return optional<nvector<double> >(dy);
      }
      
    private:
      const std::shared_ptr<chart> chart_;
      const tangent_flow curve_;
    };
    
    optional<cache_iterator_type> advance_on_chart(const std::shared_ptr<chart>& c, const nvector<double>& v, const double& from_t, const double& to_t) const
    {
      optional<std::shared_ptr<tangent_vector> > dest_tangent(curve_(to_t));
      if (!dest_tangent)
        return optional<cache_iterator_type>();
      
      bulirsch_stoer<nvector<double>, modified_midpoint_stepper> solver(parallel_transport_callback(c, curve_), from_t, v);
      
      if (!solver.step(to_t))
        return optional<cache_iterator_type>();
      
      return optional<cache_iterator_type>(cache_->insert(cache_value_type(to_t, std::shared_ptr<tangent_vector>(new tangent_vector((*dest_tangent)->context(), c, solver.y())))).first);
    }
    
    const static size_t max_recursion_ = 7;
  };
}
