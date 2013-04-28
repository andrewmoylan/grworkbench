#pragma once

#include <map>

#include <boost/optional.hpp>
#include <memory>
#include <lift/constant_iterator.hpp>
#include "approx_equal.hpp"
#include "derivative.hpp"
#include "point.hpp"
#include "nvector_differential.hpp"

namespace grwb
{
  class tangent_vector
  {
  public:
    tangent_vector(const std::shared_ptr<point> & p, const std::shared_ptr<chart>& c, const nvector<double>& v)
      : point_(p)
    {
      _.insert(make_pair(c, optional<nvector<double> >(v)));
    }

    const optional<nvector<double> >& operator[](const std::shared_ptr<chart>& c) const
    {
			const std::map<std::shared_ptr<chart>, optional<nvector<double> > >::const_iterator cached(_.find(c));
      if (cached != _.end())
	      return cached->second;
      else
	      for (std::map<std::shared_ptr<chart>, optional<nvector<double> > >::const_iterator i(_.begin()); i != _.end(); ++i)
	        if (i->second)
	        {
	          std::map<pair<std::shared_ptr<chart>, std::shared_ptr<chart> >, std::shared_ptr<inter_chart_map> >::const_iterator j(inter_chart_maps().find(make_pair(i->first, c)));
	          if (j != inter_chart_maps().end())
            {
              nvector<double> x(*(*point_)[i->first]);
							nvector<differential<double, nvector<double> > > y(make_gradient(x));
							optional<nvector<differential<double, nvector<double> > > > z((*j->second)(y));
							if (z)
							{
								nvector<nvector<double> > Z(gradient(*z));
								nvector<double> q(inner_product(Z, *i->second));
								return _.insert(make_pair(c, q)).first->second;
							}
            }
	        }
      return _.insert(make_pair(c, optional<nvector<double> >())).first->second;
    }
    
    const std::shared_ptr<point> & context() const
    {
      return point_;
    }

    tangent_vector operator+() const
    {
      return *this;
    }

    tangent_vector operator-() const
    {
      tangent_vector right(*this);
      for (iterator i(right._.begin()); i != right._.end(); ++i)
        if (i->second)
          *i->second = - *i->second;
      return right;
    }

    tangent_vector& operator+=(const tangent_vector& right)
    {
      for (iterator i(_.begin()); i != _.end(); ++i)
      {
        const optional<nvector<double> > r(right[i->first]);
        if (i->second && r)
          *i->second += *r;
      }
      return *this;
    }

    tangent_vector& operator-=(const tangent_vector& right)
    {
      for (iterator i(_.begin()); i != _.end(); ++i)
      {
        const optional<nvector<double> > r(right[i->first]);
        if (i->second && r)
          *i->second -= *r;
      }
      return *this;
    }

    template <class T> tangent_vector& operator*=(const T& x)
    {
      for (iterator i(_.begin()); i != _.end(); ++i)
        if (i->second)
          *i->second *= x;
      return *this;
    }

    template <class T> tangent_vector& operator/=(const T& x)
    {
      for (iterator i(_.begin()); i != _.end(); ++i)
        if (i->second)
          *i->second /= x;
      return *this;
    }

  private:
    mutable std::map<std::shared_ptr<chart>, optional<nvector<double> > > _;

    const std::shared_ptr<point> point_;

    typedef std::map<std::shared_ptr<chart>, optional<nvector<double> > >::const_iterator const_iterator;
    typedef std::map<std::shared_ptr<chart>, optional<nvector<double> > >::iterator iterator;

    tangent_vector& operator=(const tangent_vector&);
  };

  inline tangent_vector operator+(const tangent_vector& left, const tangent_vector& right)
  {
    return tangent_vector(left) += right;
  }

  inline tangent_vector operator-(const tangent_vector& left, const tangent_vector& right)
  {
    return tangent_vector(left) -= right;
  }

  double operator*(const tangent_vector& left, const tangent_vector& right);

  template <class T> inline tangent_vector operator*(const tangent_vector& left, const T& right)    
  {
    return tangent_vector(left) *= right;
  }

  template <class T> inline tangent_vector operator*(const T& left, const tangent_vector& right)
  {
    return tangent_vector(right) *= left;
  }

  template <class T> inline tangent_vector operator/(const tangent_vector& left, const T& right)
  {
    return tangent_vector(left) /= right;
  }

  inline double abs(const tangent_vector& v)
  {
    return sqrt(std::fabs(v * v));
  }

  inline tangent_vector normalise(const tangent_vector& v)
  {
    return v / abs(v);
  }

  inline tangent_vector orthonormalise(const tangent_vector& v1, const tangent_vector& v2)
  {
    return normalise((v1 * v2) * v2 - (v2 * v2) * v1);
  }

  double metric(const tangent_vector& v1, const tangent_vector& v2);

  inline double operator*(const tangent_vector& left, const tangent_vector& right)
  {
    return metric(left, right);
  }
}
