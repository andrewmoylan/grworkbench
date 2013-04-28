#pragma once

#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <memory>
#include <lift/differential.hpp>
#include "nvector.hpp"

namespace grwb
{
  using std::map;
  using std::pair;
  using std::set;
  using boost::function;
  using boost::optional;
  using std::shared_ptr;
  using lift::differential;

  typedef function<optional<nvector<nvector<differential<double, nvector<double> > > > >(const nvector<differential<double, nvector<double> > >&)> chart;
  typedef function<optional<nvector<differential<double, nvector<double> > > >(const nvector<differential<double, nvector<double> > >&)> inter_chart_map;

  inline map<pair<std::shared_ptr<chart>, std::shared_ptr<chart> >, std::shared_ptr<inter_chart_map> >& inter_chart_maps()
  {
    static map<pair<std::shared_ptr<chart>, std::shared_ptr<chart> >, std::shared_ptr<inter_chart_map> > _;
    return _;
  };

  inline set<std::shared_ptr<chart> >& charts()
  {
    static set<std::shared_ptr<chart> > _;
    return _;
  }
}
