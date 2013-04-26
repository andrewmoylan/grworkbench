#pragma once

#include <boost/optional.hpp>
#include <lift/constant_iterator.hpp>
#include <lift/differential.hpp>
#include "nvector.hpp"

namespace grwb
{

  using boost::optional;
  using lift::differential;
  using lift::gradient;
  using lift::make_constant_iterator;
  using lift::value;

  template<typename T> nvector<differential<T, nvector<T> > > make_gradient(const nvector<T>& x)
  {
    nvector<differential<T, nvector<T> > > result(x.size(), make_constant_iterator(differential<T, nvector<T> >(0, nvector<T>(x.size(), make_constant_iterator(0)))));
    for (size_t i(0); i != x.size(); ++i)
    {
      value(result[i]) = x[i];
      gradient(result[i])[i] = 1;
    }
    return result;
  }

  template<typename T, typename U> nvector<T> value(const nvector<differential<T, U> >& x)
  {
    nvector<T> result(x.size(), make_constant_iterator(value(x[0])));
    for (int i(0); i != x.size(); ++i)
    {
      result[i] = value(x[i]);
    }
    return result;
  }

  template<typename T> nvector<T> value(const nvector<differential<T, nvector<T> > >& x)
  {
    nvector<T> result(x.size(), make_constant_iterator(value(x[0])));
    for (size_t i(0); i != x.size(); ++i)
    {
      result[i] = value(x[i]);
    }
    return result;
  }

  template<typename U> nvector<nvector<double> > value(const nvector<nvector<differential<double, U> > >& x)
  {
    nvector<nvector<double> > result(x.size(), make_constant_iterator(nvector<double>(x[0].size())));
    for (size_t i(0); i != x.size(); ++i)
    {
      for (size_t j(0); j != x[i].size(); ++j)
      {
        result[i][j] = value(x[i][j]);
      }
    }
    return result;
  }

  /*
  template<typename T, typename U> nvector<U> gradient(const nvector<differential<T, U> >& x)
  {
    nvector<U> result(x.size(), make_constant_iterator(gradient(x[0])));
    for (int i(0); i != x.size(); ++i)
    {
      result[i] = gradient(x[i]);
    }
    return result;
  }
  */

  template<typename T, typename U> optional<nvector<T> > value(const optional<nvector<differential<T, U> > >& x)
  {
    return x ? optional<nvector<T> >(value(*x)) : optional<nvector<T> >();
  }

  /*
  template<typename T, typename U> nvector<nvector<U> > gradient(const nvector<nvector<differential<T, U> > >& x)
  {
    nvector<nvector<U> > result(x.size(), make_constant_iterator(nvector<U>(x[0].size(), make_constant_iterator(gradient(x[0][0])))));
    for (int i(0); i != x.size(); ++i)
    {
      for (int j(0); j != x[i].size(); ++j)
      {
        result[i][j] = gradient(x[i][j]);
      }
    }
    return result;
  }
  */

  inline nvector<nvector<double> > gradient(const nvector<differential<double, nvector<double> > >& x)
  {
    nvector<double> z(gradient(x[0]).size(), make_constant_iterator(0.));
    nvector<nvector<double> > result(x.size(), make_constant_iterator(z));
    for (size_t i(0); i != x.size(); ++i)
      result[i] = lift::gradient(x[i]);
    return result;
  }

  inline nvector<nvector<nvector<double> > > gradient(const nvector<nvector<differential<double, nvector<double> > > >& x)
  {
    nvector<nvector<nvector<double> > > result(x.size(), make_constant_iterator(nvector<nvector<double> >(x[0].size(), make_constant_iterator(nvector<double>(lift::gradient(x[0][0]).size())))));
    for (size_t i(0); i != x.size(); ++i)
      for (size_t j(0); j != x[i].size(); ++j)
        result[i][j] = lift::gradient(x[i][j]);
    return result;
  }

}
