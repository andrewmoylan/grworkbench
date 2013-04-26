//#pragma once
//
//#include <boost/function.hpp>
//#include <boost/optional.hpp>
//
//#include "geodesic.hpp"
//#include "parallel_transport.hpp"
//#include "point.hpp"
//#include "worldline.hpp"
//
//namespace grwb
//{
//  
//  
//
//  class parallel_curve_functor
//  {
//  public:
//    parallel_curve_functor(const function<optional<tangent_vector> (const double&)>& curve, const tangent_vector& direction)
//      : direction_(new parallel_transport(curve, direction))
//    {
//    }
//    
//    optional<shared_ptr<point> > operator()(const double& x) const
//    {
//      optional<tangent_vector> direction((*direction_)(x));
//      
//      if (!direction)
//        return optional<shared_ptr<point> >();
//      
//      return geodesic(*direction)(1.);
//    }
//    
//    
//  private:
//    const shared_ptr<parallel_transport> direction_;
//  };
//  
//  inline cached_worldline parallel_curve(const function<optional<tangent_vector> (const double&)>& curve, const tangent_vector& direction)
//  {
//    return make_cached_function(worldline(parallel_curve_functor(curve, direction)));
//  }
//}
