//#pragma once
//
//#include <list>
//#include <boost/shared_ptr.hpp>
//#include "pane.hpp"
//
//namespace grwb
//{
//  
//  
//  
//
//  class layered :
//    public pane, public list<shared_ptr<pane> >
//  {
//
//  public:
//
//    virtual bool mouse_move(const vector<signed int, 2>& _) const
//    {
//      const_iterator i(begin());
//      while (i != end() && !(*i)->mouse_move(_))
//        ++i;
//      return i != end();
//    }
//
//    virtual bool mouse_down(const vector<signed int, 2>& _)
//    {
//      iterator i(begin());
//      while (i != end() && !(*i)->mouse_down(_))
//        ++i;
//      return i != end();
//    }
//
//    virtual bool mouse_up(const vector<signed int, 2>& _)
//    {
//      iterator i(begin());
//      while (i != end() && !(*i)->mouse_up(_))
//        ++i;
//      return i != end();
//    }
//
//    virtual bool key_down(const int _)
//    {
//      iterator i(begin());
//      while (i != end() && !(*i)->key_down(_))
//        ++i;
//      return i != end();
//    }
//
//    virtual bool key_up(const int _)
//    {
//      iterator i(begin());
//      while (i != end() && !(*i)->key_up(_))
//        ++i;
//      return i != end();
//    }
//
//    virtual void draw(const vector<unsigned int, 2>& _) const
//    {
//      for (const_reverse_iterator i(rbegin()); i != rend(); ++i)
//        (*i)->draw(_);
//    }
//
//    virtual vector<unsigned int, 2> size() const
//    {
//      vector<unsigned int, 2> out;
//      for (const_iterator i(begin()); i != end(); ++i)
//      {
//        vector<unsigned int, 2> current = (*i)->size();
//        if (current[0] > out[0])
//          out[0] = current[0];
//        if (current[1] > out[1])
//          out[1] = current[1];
//      }
//      return out;
//    }
//
//  };
//
//}
