//#pragma once
//
//#include <cmath>
//#include <lift/sqr.hpp>
//#include "constants.hpp"
//#include "geodesic.hpp"
//
//namespace grwb
//{
//  inline geodesic kerr_circular_orbit(const shared_ptr<kerr>& k, const double& r, const bool& corotating = true)
//  {
//    const shared_ptr<chart>& c(*k->charts.begin());
//    const nvector<double> x(make_vector(0., r, half_pi, half_pi));
//    const ntensor_components<3>::type con(*connection(*c)(x));
//
//    return geodesic(normalise(tangent_vector(shared_ptr<point>(new point(k, c, x)), c, make_vector(1., 0., 0., (- con[1][0][3] + (corotating ? -1. : 1.) * sqrt(sqr(con[1][0][3]) - con[1][3][3] * con[1][0][0])) / con[1][3][3]))));
//  }
//}
