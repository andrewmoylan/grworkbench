//#pragma once
//
//#include "nvector.hpp"
//#include "gui.hpp"
//
//namespace grwb
//{
//  
//  class components_stub : public box
//  {
//  public:
//    explicit components_stub(size_t);
//    nvector<double> value;
//  };
//
//  inline components_stub::components_stub(const size_t n)
//    : value(n)
//  {
//    for (nvector<double>::iterator i = value.begin(); i != value.end(); ++i)
//      Slider("?", i, optional<double>(), optional<double>(), 1);
//  }
//
//}
