//#pragma once
//
//#include <fstream>
//
//#include "numerical_experiment.hpp"
//#include "kerr.hpp"
//#include "schwarzschild.hpp"
//
//namespace grwb
//{
//  template <class T> class geodesic_interferometer : public numerical_experiment<T>
//  {
//  public:
//    explicit geodesic_interferometer(const shared_ptr<T>& _);
//
//  private:
//    void reflect(const double& r, const double& interferometer_speed, const double& arm_length);
//  };
//
//  template <class T> class karim_interferometer : public numerical_experiment<T>
//  {
//  public:
//    explicit karim_interferometer(const shared_ptr<T>& _);
//
//  private:
//    void reflect(const double& r, const double& interferometer_speed, const double& arm_length);
//  };
//}
//
//#include "constants.hpp"
//#include "connecting_null_geodesic.hpp"
//#include "coordinate_line.hpp"
//#include "geodesic.hpp"
//#include "geodesic_tangent.hpp"
//#include "metric.hpp"
//#include "parallel_curve.hpp"
//#include "worldline.hpp"
//#include "curve_stub.hpp"
//#include "kerr_circular_orbit.hpp"
//#include "linear_distortion.hpp"
//
//namespace grwb
//{
//  template <class T> inline karim_interferometer<T>::karim_interferometer(const shared_ptr<T>& _)
//    : numerical_experiment<T>(_)
//  {
////    this->add_distortion("Spherical to Orthonormal");
// //   this->add_distortion("Linear");
//    nvector<nvector<double> >& lin(*boost::dynamic_pointer_cast<linear_distortion>(this->distortions.back())->matrix);
//    lin[0][0] = 0.;
//    lin[0][3] = 1.;
//
//    reflect(10., 0.2, 3.);
//
//    //for (double r(3.); r <= 50.; r *= sqrt(sqrt(2.)))
//    //  reflect(r, 0.01, 1.);
//
//    //for (double l(0.01); l <= 6.; l *= sqrt(sqrt(2.)))
//    //  reflect(10., 0.01, l);
//
//    //for (double v(0.001); v <= 0.5; v *= sqrt(sqrt(2.)))
//    //  reflect(10., v, 1.);
//
//    //for (double r(3.); r <= 50.; r *= sqrt(sqrt(2.)))
//    //  reflect(r, 0., 1.);
//
//    //for (double l(0.01); l <= 6.; l *= sqrt(sqrt(2.)))
//    //  reflect(10., 0., l);
//
//    //exit(EXIT_SUCCESS);
//  }
//
//  template <class T> inline geodesic_interferometer<T>::geodesic_interferometer(const shared_ptr<T>& _)
//    : numerical_experiment<T>(_)
//  {
////    this->add_distortion("Spherical to Orthonormal");
//  //  this->add_distortion("Linear");
//    nvector<nvector<double> >& lin(*boost::dynamic_pointer_cast<linear_distortion>(this->distortions.back())->matrix);
//    lin[0][0] = 0.;
//    lin[0][3] = 1.;
//
//    reflect(10., 0.2, 2.);
//
//    //for (double r(3.); r <= 50.; r *= sqrt(sqrt(2.)))
//    //  reflect(r, 0.01, 1.);
//
//    //for (double l(0.01); l <= 6.; l *= sqrt(sqrt(2.)))
//    //  reflect(10., 0.01, l);
//
//    //for (double v(0.001); v <= 0.5; v *= sqrt(sqrt(2.)))
//    //  reflect(10., v, 1.);
//
//    //for (double r(3.); r <= 50.; r *= sqrt(sqrt(2.)))
//    //  reflect(r, 0., 1.);
//
//    //for (double l(0.01); l <= 6.; l *= sqrt(sqrt(2.)))
//    //  reflect(10., 0., l);
//
//    //exit(EXIT_SUCCESS);
//  }
//
//  template <class T> inline void geodesic_interferometer<T>::reflect(const double& r, const double& interferometer_speed, const double& arm_length)
//  {
//    const double two_arm_length(2. * arm_length);
//
//    const shared_ptr<chart>& c(this->any_chart());
//
//    const nvector<double> coordinate_direction(make_vector(1., 0., 0., interferometer_speed / r));
//
//    const point origin(this->atlas(), c, make_vector(0., r, half_pi, half_pi));
//    const tangent_vector tangent(normalise(tangent_vector(origin, c, coordinate_direction)));
//    const cached_worldline beam_splitter(coordinate_line(tangent, c));
//
//    cout << endl << "Beam-splitter origin = " << *origin[c] << ", tangent = " << *tangent[c] << "." << endl;
//    cout << "Interferometer speed: " << interferometer_speed << endl;
//    cout << "Interferometer arm length: " << arm_length << "." << endl;
//    //output << r << endl;
//    //output << interferometer_speed << endl;
//    //output << arm_length << endl;
//
//    const tangent_vector radial_mirror_direction(orthonormalise(tangent_vector(origin, c, make_vector(0., 1., 0., 0.)), tangent));
//    const tangent_vector theta_mirror_direction(orthonormalise(orthonormalise(tangent_vector(origin, c, make_vector(0., 0., 1., 0.)), tangent), radial_mirror_direction));
//    const tangent_vector phi_mirror_direction(orthonormalise(orthonormalise(orthonormalise(tangent_vector(origin, c, make_vector(0., 0., 0., 1.)), tangent), radial_mirror_direction), theta_mirror_direction));
//    const vector<tangent_vector, 5> directions(make_vector(radial_mirror_direction, -radial_mirror_direction, theta_mirror_direction, phi_mirror_direction, -phi_mirror_direction));
//    
//    double max_affine_length(0.);
//
//    for (const tangent_vector* i(directions.begin()); i != directions.end(); ++i)
//    {
//      const point mirror_origin(*geodesic(*i)(arm_length));
//      const tangent_vector mirror_tangent(normalise(tangent_vector(mirror_origin, c, coordinate_direction)));  
//      const cached_worldline mirror(coordinate_line(mirror_tangent, c));
//
//      const geodesic outward_ray(connecting_null_geodesic(origin, mirror, arm_length)->second);
//      const point reflection(*outward_ray(1.));
//      
//      const pair<double, geodesic> con(*connecting_null_geodesic(reflection, beam_splitter, two_arm_length));
//      const double& affine_length(con.first);
//      const geodesic& inward_ray(con.second);
//
//      this->plot(mirror, 0., affine_length);
//      this->plot(outward_ray, 0., 1.);
//      this->plot(inward_ray, 0., 1.);
//
//      cout << "  Mirror direction = " << *(*i)[c] << ", time experienced by beam splitter = " << affine_length << "." << endl;
//      cout << "    Outward ray tangent = " << *(*outward_ray.tangent(0.))[c] << endl;
//      cout << "    Inward ray tangent = " << *(*inward_ray.tangent(0.))[c] << endl;      
//      //output << affine_length << endl;
//
//      if (affine_length > max_affine_length)
//        max_affine_length = affine_length;
//    }
//
//    this->plot(beam_splitter, 0., max_affine_length);
//  }
//
//  template <class T> inline void karim_interferometer<T>::reflect(const double& r, const double& interferometer_speed, const double& arm_length)
//  {
//    const double two_arm_length(2. * arm_length);
//
//    const shared_ptr<chart>& c(this->any_chart());
//
//    const nvector<double> coordinate_direction(make_vector(1., 0., 0., interferometer_speed / r));
//
//    const point origin(this->atlas(), c, make_vector(0., r, half_pi, half_pi));
//    const tangent_vector tangent(normalise(tangent_vector(origin, c, coordinate_direction)));
//    const cached_worldline beam_splitter(coordinate_line(tangent, c));
//
//    cout << endl << "Beam-splitter origin: " << *origin[c] << ", tangent: " << *tangent[c] << "." << endl;
//    cout << "Interferometer speed: " << interferometer_speed << endl;
//    cout << "Interferometer arm length: " << arm_length << "." << endl;
//    //output << r << endl;
//    //output << interferometer_speed << endl;
//    //output << arm_length << endl;
//
//    const tangent_vector radial_mirror_direction(origin, c, make_vector(0., 1., 0., 0.));
//    const tangent_vector theta_mirror_direction(origin, c, make_vector(0., 0., 1. / r, 0.));
//    const tangent_vector phi_mirror_direction(origin, c, make_vector(0., 0., 0., 1. / r));
//    const vector<tangent_vector, 5> directions(make_vector(radial_mirror_direction, -radial_mirror_direction, theta_mirror_direction, phi_mirror_direction, -phi_mirror_direction));
//
//    double max_affine_length(0.);
//
//    for (const tangent_vector* i(directions.begin()); i != directions.end(); ++i)
//    {
//      const point mirror_origin(*coordinate_line(*i, c)(arm_length));
//      const tangent_vector mirror_tangent(normalise(tangent_vector(mirror_origin, c, coordinate_direction)));
//      const cached_worldline mirror(coordinate_line(mirror_tangent, c));
//
//      const geodesic outward_ray(connecting_null_geodesic(origin, mirror, arm_length)->second);
//      const point reflection(*outward_ray(1.));
//      
//      const pair<double, geodesic> con(*connecting_null_geodesic(reflection, beam_splitter, two_arm_length));
//      const double& affine_length(con.first);
//      const geodesic& inward_ray(con.second);
//
//      this->plot(mirror, 0., affine_length);
//      this->plot(outward_ray, 0., 1.);
//      this->plot(inward_ray, 0., 1.);
//
//      cout << "  Mirror direction = " << *(*i)[c] << ", time experienced by beam splitter = " << affine_length << "." << endl;
//      cout << "    Outward ray tangent = " << *(*outward_ray.tangent(0.))[c] << endl;
//      cout << "    Inward ray tangent = " << *(*inward_ray.tangent(0.))[c] << endl;      
//      //output << affine_length << endl;
//
//      if (affine_length > max_affine_length)
//        max_affine_length = affine_length;
//    }
//
//    this->plot(beam_splitter, 0., max_affine_length);
//  }
//}
