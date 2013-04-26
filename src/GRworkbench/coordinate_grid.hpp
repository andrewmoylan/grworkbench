//#pragma once
//
//#include <list>
//#include "object.h"
//#include "curve.h"
//#include "geodesic.h"
//#include "geodesic_launcher.h"
//
//namespace grwb
//{
//
//  class coordinate_grid : public object
//  {
//  public:
//    coordinate_grid::coordinate_grid(const geodesic_launcher& gl)
//      : _(gl)
//    {
//    }
//    
//    virtual void draw(const view& v) const
//    {
//      static optional<nvector<double> > old_x;
//      optional<shared_ptr<point> > o(_.origin());
//      if (!o || !old_x || (*o)[o->valid_chart()] != old_x)
//      {
//	curves_.clear();
//	
//	if (o)
//	{
//	  old_x = (*o)[o->valid_chart()];
//	  
//	  int dim((*o)[o->valid_chart()]->size());
//	  v.draw_point(*o);
//	  for (int i(0); i < dim; ++i)
//	  {
//	    nvector<double> t(dim);
//	    t[i] = 1;
//	    tangent_vector tv(*o, v.cartographer, t);
//	    geodesic g(tv);
//	    curves_.push_back(shared_ptr<curve>(new curve(g, -1. / abs(tv), 1. / abs(tv))));
//	  }
//	}
//	else
//	  old_x.reset();
//      }
//
//      if (o)
//	for (list<shared_ptr<curve> >::const_iterator i(curves_.begin()); i != curves_.end(); ++i)
//	  (*i)->draw(v);
//    }
//    
//  private:
//    const geodesic_launcher& _;
//    mutable list<shared_ptr<curve> > curves_;
//  };
//}
//
