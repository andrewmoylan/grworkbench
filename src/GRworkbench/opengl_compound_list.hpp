#pragma once

#include "opengl.hpp"
#include <list>

namespace grwb
{
  using namespace std;

  class opengl_compound_list
  {
  public:
    ~opengl_compound_list()
    {
      clear();
    }

    void push_back(const GLint l)
    {
      lists_.push_back(l);
    }

    void clear()
    {
      for (list<GLint>::const_iterator i(lists_.begin()); i != lists_.end(); ++i)
        glDeleteLists(*i, 1);
      lists_.clear();
    }

    void draw() const
    {
      for (list<GLint>::const_iterator i(lists_.begin()); i != lists_.end(); ++i)
        glCallList(*i);
    }

  private:
    list<GLint> lists_;
  };
}
