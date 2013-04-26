//#pragma once
//
//#include <boost/optional.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/weak_ptr.hpp>
//#include "object_stub.hpp"
//
//namespace grwb
//{
//
//  class CStub;
//  class caterpillar;
//  class view;
//
//  class caterpillar_stub : public object_stub
//  {
//  public:
//    caterpillar_stub(const shared_ptr<caterpillar>& c, view& a, const shared_ptr<CStub>& s);
//
//  private:
//    weak_ptr<caterpillar> _;
//  };
//}
//
//#include "caterpillar.hpp"
//#include "gui.hpp"
//
//namespace grwb
//{
//  
//  inline caterpillar_stub::caterpillar_stub(const boost::shared_ptr<caterpillar>& c, view& a, const boost::shared_ptr<CStub>& s)
//    : object_stub(c, a, s),
//      _(c)
//  {
//    set_name("(" + get_name() + ")_Caterpillar");
//    
//    SliderDouble("Start", &c->begin, optional<double>(), optional<double>(), 10);
//    SliderDouble("End", &c->end, optional<double>(), optional<double>(), 10);
//    Slider("Points", &c->samples, optional<double>(1.), optional<double>(1000.), 10);
//    Separator();
//  }
//}
