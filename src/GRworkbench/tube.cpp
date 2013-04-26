//#include <boost/optional.hpp>
//#include <boost/shared_ptr.hpp>
//#include <lift/registry.hpp>
//#include <lift/vector.hpp>
//#include "nvector.hpp"
//#include "atlas.hpp"
//#include "tensor.hpp"
//
//
//namespace grwb
//{
//
//  namespace tube 
////    : public atlas
//  {
//
////  public:
//
//  //  tube()
//  ////    : atlas(2)
//  //  {
//  //    shared_ptr<chart> alpha(new chart(cartesian()));
//  //    charts.insert(alpha);
//  //  }
//
////    class view 
////      : public grwb::view
////    {
////
////    public:
////
////      view(const shared_ptr<tube>& _)
////        : grwb::view(_), atlas_(_)
////      {
//////        stub->set_name("Tube");
////      }
////
////      virtual ~view() 
////      {
////      }
////
////      virtual void draw() const
////      {
////        grwb::view::draw();
////      }
////
////      const shared_ptr<tube>& tube_atlas() const
////      {
////        return atlas_;
////      }
////
////    private:
////
////      shared_ptr<tube> atlas_;
////
////    };
//
////  private:
//
//    class cartesian
//    {
//
//    public:
//
//      cartesian()
//      {
//      }
//
//      optional<nvector<nvector<differential<double, nvector<double> > > > > operator()(const nvector<differential<double, nvector<double> > >& x) const
//      {
//        const differential<double, nvector<double> > r(sqrt(sqr(x[0]) + sqr(x[1])));
//
//        if (r == 0.)
//          return optional<nvector<nvector<differential<double, nvector<double> > > > > ();
//
//        const differential<double, nvector<double> > inverse_r(1. / r);
//
//        nvector<nvector<differential<double, nvector<double> > > > result(2, make_constant_iterator(nvector<differential<double, nvector<double> > >(2, make_constant_iterator(differential<double, nvector<double> >(0, nvector<double>(2, make_constant_iterator(0)))))));
//
//        result[0][0] = inverse_r;
//        result[1][1] = inverse_r;
//
//        return result;
//      }
//
//    };
//
//  }
//
//  /*namespace
//  {
//
//    lift::registrar<view, tube::view> tube_registerer("Tube", boost::shared_ptr<tube>(new tube()));
//
//  }*/
//
//}
//
