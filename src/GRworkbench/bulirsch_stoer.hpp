#pragma once

#include <map>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <lift/vector.hpp>
#include <lift/elementary.hpp>
#include "richardson_extrapolation.hpp"
#include "relative_difference.hpp"
#include "approx_equal.hpp"
#include "bulirsch_stoer_parameters.hpp"

namespace grwb
{

  using std::cout;
  using std::endl;
  using lift::vector;
  using lift::sqr;

  template <class T, template <class> class U> class bulirsch_stoer
    // adapted from Numerical Recipes in C (2nd Edition), p. 728
  {
  public:
    bulirsch_stoer(const function<optional<T> (const double&, const T&)>& f, const double& x_0, const T& y_0, const double& default_stepsize = 1., const size_t& maximum_steps = 100, const double& relative_error = default_approx_equal_tolerance)
      : 
    safe1_(0.25),
      safe2_(0.7),
      max_stepsize_reduction_(1.e-5),
      min_stepsize_reduction_(0.7),
      max_stepsize_increase_(0.1),
      f_(f),	maximum_steps_(maximum_steps),
      relative_error_(relative_error),
      default_h_(default_stepsize),
      x_(x_0),
      y_(y_0),
			safe3_(1e-14)
    {
      const double safe_relative_error(relative_error * safe1_);

      typename std::map<double, vector<vector<double, bulirsch_stoer_parameters<U>::k_total>, bulirsch_stoer_parameters<U>::k_total> >::const_iterator i(alpha_cache_().find(safe_relative_error));
      if (i != alpha_cache_().end())
        alpha_ = i->second;
      else
      {
        for (size_t i = 1; i < bulirsch_stoer_parameters<U>::k_total; ++i)
          for (size_t j = 0; j < i; ++j)
            alpha_[j][i] = pow(safe_relative_error, (a_()[j] - a_()[i]) / ((a_()[i] - a_()[0] + 1.) * (2 * j + 3)));
        alpha_cache_()[safe_relative_error] = alpha_;
      }

      for (optimal_k_ = 1; optimal_k_ < bulirsch_stoer_parameters<U>::k_total - 1; ++optimal_k_)
        if (a_()[optimal_k_ + 1] > a_()[optimal_k_] * alpha_[optimal_k_ - 1][optimal_k_])
          break;
      max_k_ = optimal_k_;
    }

    const double& x() const
    {
      return x_;
    }

    const T& y() const
    {
      return y_;
    }

    bool step(const double& to_x)
    {
      double h(default_h_);
      if (to_x < x_)
        h *= -1;

      for (size_t i = 0; i < maximum_steps_; ++i)
      {
        bool reduced_step_size(false);
        bool success(false);
        size_t k(0), km(0);
        double stepsize_reduction_factor(0.);
        double err[bulirsch_stoer_parameters<U>::k_total];
        U<T> stepper(f_, x_, y_);

        if (to_x == x_)
          return true;

        if ((to_x - x_) * (to_x - x_ - h) < 0.)
          h = to_x - x_;

        for (;;)
        {
          optional<richardson_extrapolation<T> > extrapolator;

          for (k = 0; k < max_k_; ++k)
          {
            optional<T> y_est(stepper.step(h, bulirsch_stoer_parameters<U>::k_values[k]));
            if (!y_est)
              return false;

            const double little_h_squared(sqr(h / bulirsch_stoer_parameters<U>::k_values[k]));
            if (!extrapolator)
              extrapolator.reset(richardson_extrapolation<T>(little_h_squared, *y_est));
            else
            {
              extrapolator->refine(little_h_squared, *y_est);
              y_ = extrapolator->limit();

              const double error(relative_difference(y_, y_ + extrapolator->error()) / relative_error_);
              km = k - 1;
              err[km] = pow(error / safe1_, 1. / (2 * km + 3));

              if (k >= optimal_k_ - 1 || i == 0)
              {
                if (error < 1.)
                {
                  success = true;
                  break;
                }
                if (k == max_k_ || k == optimal_k_ + 1)
                {
                  stepsize_reduction_factor = safe2_ / err[km];
                  break;
                }
                if (k == optimal_k_ && alpha_[optimal_k_ - 1][optimal_k_] < err[km])
                {
                  stepsize_reduction_factor = 1. / err[km];
                  break;
                }
                if (optimal_k_ == max_k_ && alpha_[km][max_k_ - 1] < err[km])
                {
                  stepsize_reduction_factor = alpha_[km][max_k_ - 1] * safe2_ / err[km];
                  break;
                }
                if (alpha_[km][optimal_k_] < err[km])
                {
                  stepsize_reduction_factor = alpha_[km][optimal_k_ - 1] / err[km];
                  break;
                }
              }
            }
          }

          if (success)
            break;

          if (stepsize_reduction_factor > min_stepsize_reduction_)
            stepsize_reduction_factor = min_stepsize_reduction_;
          if (stepsize_reduction_factor < max_stepsize_reduction_)
            stepsize_reduction_factor = max_stepsize_reduction_;
          h *= stepsize_reduction_factor;
          reduced_step_size = true;
        }

        x_ += h;

        double work_min(1.e300);
        double scale_factor(0.);
        for (size_t j = 0; j <= km; ++j)
        {
          const double s(err[j] < max_stepsize_increase_ ? max_stepsize_increase_ : err[j]);
          const double work(s * a_()[j + 1]);
          if (work < work_min)
          {
            scale_factor = s;
            work_min = work;
            optimal_k_ = j + 1;
          }
        }
        if (optimal_k_ >= k && optimal_k_ != max_k_ && !reduced_step_size)
        {
          double s(scale_factor / alpha_[optimal_k_ - 1][optimal_k_]);
          if (s < max_stepsize_increase_)
            s = max_stepsize_increase_;

          if (a_()[optimal_k_ + 1] * s <= work_min)
          {
            scale_factor = s;
            ++optimal_k_;
          }
        }
        h /= scale_factor;
      }

      //log_cout << "Bulirsch-Stoer: Too many steps required." << endl;
      return false;
    }

  private:
    const double safe1_;
    const double safe2_;
		const double safe3_;
    const double max_stepsize_reduction_;
    const double min_stepsize_reduction_;
    const double max_stepsize_increase_;

    const function<optional<T> (const double&, const T&)> f_;
    const size_t maximum_steps_;
    const double relative_error_;
    const double default_h_;

    vector<vector<double, bulirsch_stoer_parameters<U>::k_total>, bulirsch_stoer_parameters<U>::k_total> alpha_;

    static std::map<double, vector<vector<double, bulirsch_stoer_parameters<U>::k_total>, bulirsch_stoer_parameters<U>::k_total> >& alpha_cache_()
    {
      static std::map<double, vector<vector<double, bulirsch_stoer_parameters<U>::k_total>, bulirsch_stoer_parameters<U>::k_total> > _;
      return _;
    };

    static vector<double, bulirsch_stoer_parameters<U>::k_total + 1>& a_()
    {
      static optional<vector<double, bulirsch_stoer_parameters<U>::k_total + 1> > _;
      if (!_)
      {
        _.reset(vector<double, bulirsch_stoer_parameters<U>::k_total + 1>());
        (*_)[0] = bulirsch_stoer_parameters<U>::k_values[0] + 1;
        for (size_t i = 0; i < bulirsch_stoer_parameters<U>::k_total; ++i)
          (*_)[i + 1] = (*_)[i] + bulirsch_stoer_parameters<U>::k_values[i + 1];
      }
      return *_;
    }

    double x_;
    T y_;
    size_t optimal_k_;
    size_t max_k_;
    bulirsch_stoer& operator=(const bulirsch_stoer&);
  };
}
