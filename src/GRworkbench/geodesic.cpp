#include "geodesic.hpp"
#include <map>

#include <boost/optional.hpp>
#include <memory>
#include <boost/weak_ptr.hpp>
#include "atlas.hpp"
#include "bulirsch_stoer.hpp"
#include <lift/constant_iterator.hpp>
#include "modified_midpoint_stepper.hpp"
#include "connection.hpp"
#include <limits>

namespace grwb
{ 
  const double positive_infinity(std::numeric_limits<double>::infinity());
  const double negative_infinity(-std::numeric_limits<double>::infinity());
	const size_t geodesic::max_recursion_ = 15;

  geodesic::geodesic(const tangent_vector& t) : least_upper_bound_(positive_infinity), greatest_lower_bound_(negative_infinity), cache_(new std::map<double, std::shared_ptr<tangent_vector> >)
  {
    cache_->insert(cache_value_type(0, std::shared_ptr<tangent_vector>(new tangent_vector(t))));
  }

  optional<std::shared_ptr<point> > geodesic::value(const double& t) const
  {
    return operator()(t, 0.).second;
  }

  pair<double, optional<std::shared_ptr<point> > > geodesic::operator()(const double& t, const double& epsilon) const
  {
    typedef pair<double, optional<std::shared_ptr<point> > > return_type;

    if (t >= least_upper_bound_)
      return return_type(least_upper_bound_, optional<std::shared_ptr<point> >());
    else if (t <= greatest_lower_bound_)
      return return_type(greatest_lower_bound_, optional<std::shared_ptr<point> >());

    cache_iterator_type initial_data(get_initial_data(t));

    if (fabs(initial_data->first - t) <= epsilon)
      return return_type(initial_data->first, optional<std::shared_ptr<point> >(initial_data->second->context()));

    optional<cache_iterator_type> result(advance(*initial_data->second, initial_data->first, t));

    if (result)
      return return_type(t, optional<std::shared_ptr<point> >((*result)->second->context()));

    if (t > 0)
    {
      least_upper_bound_ = t;
      //log_cout << "Least upper bound set to " << t << ".\n";
    }
    else
    {
      greatest_lower_bound_ = t;
      //log_cout << "Greatest lower bound set to " << t << ".\n";
    }

    return return_type(t, optional<std::shared_ptr<point> >());
  }

  optional<std::shared_ptr<tangent_vector> > geodesic::operator()(const double& t) const
  {
    if (!value(t))
      return optional<std::shared_ptr<tangent_vector> >();

    return cache_->find(t)->second;
  }

  geodesic::geodesic_callback::geodesic_callback(const std::shared_ptr<chart>& c)
    : _(c)
  {
  }

  optional<vector<nvector<double>, 2> > geodesic::geodesic_callback::operator()(const double&, const vector<nvector<double>, 2>& y) const
  {
    const int dim(static_cast<int>(y[0].size()));

    vector<nvector<double>, 2> ret(make_constant_iterator(y[1]));

    optional<nvector<nvector<nvector<double> > > > con(connection(*_)(y[0]));
    if (!con)
      return optional<vector<nvector<double>, 2> >();

    ret[1] *= 0.;
    for (int a = 0; a < dim; ++a)
      for (int b = 0; b < dim; ++b)
        for (int c = 0; c < dim; ++c)
          ret[1][a] -= (*con)[a][b][c] * y[1][b] * y[1][c];

    return optional<vector<nvector<double>, 2> >(ret);
  }

  geodesic::cache_iterator_type geodesic::get_initial_data(const double& t) const
  {
    cache_iterator_type after(cache_->lower_bound(t));

    if (after == cache_->begin())
      return after;

    if (after == cache_->end())
      return --cache_iterator_type(after);

    cache_iterator_type before(after);
    --before;
    return fabs(before->first - t) < fabs(after->first - t) ? before : after;
  }

  optional<geodesic::cache_iterator_type> geodesic::advance(const tangent_vector& tangent, const double& from_t, const double& to_t, size_t recursion) const
  {
    const std::shared_ptr<point>& origin(tangent.context());

    if (recursion > max_recursion_)
      return optional<cache_iterator_type>();

    //log_cout << "  Geodesic: " << from_t << " -> " << to_t << endl;

    for (set<std::shared_ptr<chart> >::const_iterator i(charts().begin()++); i != charts().end(); ++i)
      if ((*origin)[*i] && tangent[*i])
      {
        optional<cache_iterator_type> result(advance_on_chart(*i, *(*origin)[*i], *tangent[*i], from_t, to_t));
        if (result)
          return result;
      }

      optional<cache_iterator_type> halfway(advance(tangent, from_t, (from_t + to_t) / 2, recursion + 1));
      return halfway ? advance(*(*halfway)->second, (from_t + to_t) / 2, to_t, recursion + 1) : optional<cache_iterator_type>();
  }

  using lift::make_vector;

  optional<geodesic::cache_iterator_type> geodesic::advance_on_chart(const std::shared_ptr<chart>& c, const nvector<double>& x, const nvector<double>& dx, const double& from_t, const double& to_t) const
  {
    bulirsch_stoer<vector<nvector<double>, 2>, modified_midpoint_stepper> solver((geodesic_callback(c)), from_t, make_vector(x, dx));

    if (!solver.step(to_t))
      return optional<cache_iterator_type>();

    std::shared_ptr<point> dest(new point(c, solver.y()[0]));
    tangent_vector tv(dest, c, solver.y()[1]);

    return optional<cache_iterator_type>(cache_->insert(cache_value_type(to_t, std::shared_ptr<tangent_vector>(new tangent_vector(tv)))).first);
  }
}
