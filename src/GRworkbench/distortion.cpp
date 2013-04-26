/*
#include <lift/registry.hpp>
#include "distortion.hpp"

namespace grwb 
{

  distortion::distortion()
    : display(0)//,  stub_() 
  {
  }
   
  distortion::~distortion() 
  {
    if (glIsList(display))
      glDeleteLists(display, 1);    
//    if (stub_)
//      stub_->Hide();
  }
*/
  /*
//  shared_ptr<box> distortion::stub() const 
//  {
//    if (!stub_) 
//    {
//      stub_ = shared_ptr<box>(new box);
//      stub_->set_name(name());
//      stub_->Separator();
//    }
//    return stub_;
//  }
  
  void distortion::cached_draw() const 
  {
    if (glIsList(display)) 
      glCallList(display);
    else
    {
      display = glGenLists(1);
      glNewList(display, GL_COMPILE_AND_EXECUTE);
        draw();
      glEndList();
    }
  }

  void distortion::invalidate() 
  {
    if (glIsList(display))
      glDeleteLists(display, 1);
    display = 0;
  }
}*/  // grwb
