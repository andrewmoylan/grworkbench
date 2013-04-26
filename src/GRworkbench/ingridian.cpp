//#include <lift/registry.hpp>
//#include <lift/vector.hpp>
//#include "constants.hpp"
//#include "atlas.hpp"
//#include "tensor.hpp"
//
//
//namespace grwb
//{
//
//    namespace ingridian 
////      : public atlas
//    {
//
//    //public:
//
//    //  ingridian();
//
////      class view 
////        : public grwb::view
////      {
////
////      public:
////
////        view(const shared_ptr<ingridian>& _)
////          : grwb::view(_)
////        {
//////          stub->set_name("Ingridian");
////        }
////
////        virtual ~view() 
////        {
////        }
////
////        virtual void draw() const
////        {
////          grwb::view::draw();
////        }
////
////      };
//
////    private:
//
//      class chart
//      {
//
//      public:
//
//        optional<nvector<nvector<differential<double, nvector<double> > > > > operator()(const nvector<differential<double, nvector<double> > >&) const;
//
//      };
//
//    }
//
//    using lift::sqr;
//
//    optional<nvector<nvector<differential<double, nvector<double> > > > > ingridian::chart::operator()(const nvector<differential<double, nvector<double> > >& x) const
//    {
//
//      if (x[1] >= two_pi || x[1] <= 0. || x[0] <= 0.)
//      {
//        return optional<nvector<nvector<differential<double, nvector<double> > > > >();
//      }
//
//        nvector<nvector<differential<double, nvector<double> > > > result(2, make_constant_iterator(nvector<differential<double, nvector<double> > >(2, make_constant_iterator(differential<double, nvector<double> >(0, nvector<double>(2, make_constant_iterator(0)))))));
//
//      result[0][0] = -1. / x[1];
//      result[1][1] = sqr(x[0]);
//
//      return result;
//
//    }
//
//    //ingridian::ingridian()
//    //  : atlas(2)
//    //{
//    //  charts.insert(shared_ptr<chart>(new chart(chart())));
//    //}
//
//    //lift::registrar<view, ingridian::view> ingridian_registerer("Ingridian", boost::shared_ptr<ingridian>(new ingridian()));
//
//
//}
