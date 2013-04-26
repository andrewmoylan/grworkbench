#pragma once

#include <vector>

namespace grwb
{
  

  inline nvector<nvector<double> > operator*(const double& left, const nvector<nvector<double> >& right)
  {
    nvector<nvector<double> > result(right);
    for (nvector<nvector<double> >::iterator i(result.begin()); i != result.end(); ++i)
      *i *= left;
    return result;
  }

  template <typename T> class richardson_extrapolation
  {
  public:
    richardson_extrapolation(const double& x, const T& y)
      : limit_(y),
	error_(y)
    {
      refine(x, y);
    }

    const T& limit() const
    {
      return limit_;
    }
    
    const T& error() const
    {
      return error_;
    }
    
    void refine(const double& x, const T& y)
    {
      // adapted from Numerical Recipes in C (2nd Edition), p. 731
      
      data_.resize(data_.size() + 1, std::make_pair(x, y));
      error_ = limit_ = y;
      
      const size_t n(data_.size());
      if (n == 1)
	return;
      
      T c(y);
      for (size_t i(1); i < n; ++i)
      {
	const double x_i(data_[n - i - 1].first);
	const double delta(1. / (x_i - x));
	const double f1(x * delta);
	const double f2(x_i * delta);
	const T q(data_[i - 1].second);
	data_[i - 1].second = error_;
	const T d2(c - q);
	error_ = f1 * d2;
	c = f2 * d2;
	limit_ += error_;
      }
      
      data_[n - 1].second = error_;
    }

  private:
    std::vector<pair<double, T> > data_;

    T limit_;
    T error_;    
  };
}
