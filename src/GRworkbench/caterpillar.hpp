//#pragma once
//
//#include <list>
//#include <boost/function.hpp>
//#include <boost/optional.hpp>
//#include <boost/shared_ptr.hpp>
//#include "object.h"
//
//namespace grwb
//{
//
//  class view;
//  class tangent_vector;
//  class curve;
//
//  class caterpillar : public object
//  {
//    friend class caterpillar_stub;
//
//  public:
//    caterpillar(const function<optional<tangent_vector> (const double&)>&, const double& = 0., const double& = 0., int = 1);
//    virtual void draw(const view&) const;
//   
//  private:
//    const function<optional<tangent_vector> (const double&)> f_;
//    
//    double begin;
//    double end;
//    int samples;
//
//    mutable list<shared_ptr<curve> > curves;
//  };
//}
//
//#include "curve.h"
//#include "geodesic.h"
//#include "tensor.h"
//#include "view.h"
//
//namespace grwb
//{
//
//  inline caterpillar::caterpillar(const function<optional<tangent_vector> (const double&)>& f, const double& a, const double& b, int s)
//    : f_(f),
//      begin(a),
//      end(b),
//      samples(s)
//  {
//  }
//
//  inline void caterpillar::draw(const view& v) const
//  {
//    static double old_begin(0.);
//    static double old_end(0.);
//    static size_t old_samples(0);
//    
//    if (begin != old_begin || end != old_end || samples != old_samples)
//    {
//      curves.clear();
//
//      old_begin = begin;
//      old_end = end;
//      old_samples = samples;
//
//      const double delta((end - begin) / samples);
//      for (double x(begin); x < end + delta; x += delta)
//      {
//        //cout << x << endl;
//        optional<tangent_vector> origin(f_(x));
//        if (origin)
//          curves.push_back(shared_ptr<curve>(new curve(geodesic(*origin), 0., 1., 1)));
//      }
//    }      
//
//    for (list<shared_ptr<curve> >::const_iterator i(curves.begin()); i != curves.end(); ++i)
//      (*i)->draw(v);
//  }
//}
