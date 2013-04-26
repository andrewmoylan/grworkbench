#pragma once

#include <lift/vector.hpp>

namespace grwb
{

  using lift::vector;

  class pane
  {

  public:

    pane() {}
    virtual ~pane() {}
    virtual bool mouse_move(const vector<signed int, 2>&) const { return false; }
    virtual bool mouse_down(const vector<signed int, 2>&) { return false; }
    virtual bool mouse_up(const vector<signed int, 2>&) { return false; }
    virtual bool key_down(const int) { return false; }
    virtual bool key_up(const int) { return false; }
    virtual void draw(const vector<unsigned int, 2>&) const {}
    virtual vector<size_t, 2> size() const { return vector<size_t, 2>(); }
		virtual void process() {}

  private:

    pane(const pane&);
    pane& operator=(const pane&);

  };

};
