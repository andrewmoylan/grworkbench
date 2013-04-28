#pragma once

#include <string>
#include <boost/optional.hpp>
#include <memory>
#include <lift/vector.hpp>
#include "nvector.hpp"
#include "opengl.hpp"
#include <lift/differential.hpp>

namespace grwb 
{

  using std::string;
  using boost::optional;
  using lift::differential;

  class distortion 
  {

  public:

    distortion() : display(0) {}
    virtual ~distortion() { if (glIsList(display)) glDeleteLists(display, 1); }

    optional<nvector<differential<double, nvector<double> > > > operator()(const optional<nvector<differential<double, nvector<double> > > >& p) const { if (p) return operator()(*p); else return p; }
    virtual optional<nvector<differential<double, nvector<double> > > > operator()(const nvector<differential<double, nvector<double> > >&) const { return optional<nvector<differential<double, nvector<double> > > >(); }
    optional<nvector<differential<double, nvector<double> > > > undistort(const optional<nvector<differential<double, nvector<double> > > >& p) const { if (p) return undistort(*p); else return p; }
    virtual optional<nvector<differential<double, nvector<double> > > > undistort(const nvector<differential<double, nvector<double> > >&) const { return optional<nvector<differential<double, nvector<double> > > >(); }
    void cached_draw() const;

  private:

    mutable GLint display;

  };

}

