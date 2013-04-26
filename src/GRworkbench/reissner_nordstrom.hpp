//#pragma once
//
//#include <lift/sqr.hpp>
//#include <lift/vector.hpp>
//#include "constants.hpp"
//#include "atlas.hpp"
//#include "tensor.hpp"
//#include "opengl.hpp"
//
//
//namespace grwb
//{
//  using lift::make_vector;
//  class reissner_nordstrom : public atlas
//  {
//  public:
//    reissner_nordstrom(const double& = 2., const double& q = sqrt(3.));
//
//    /*class view : public grwb::view
//    {
//    public:
//      view(const shared_ptr<reissner_nordstrom>&);
//      virtual ~view() {}
//      virtual void draw() const;
//
//      const shared_ptr<reissner_nordstrom>& reissner_nordstrom_atlas() const
//      {
//        return atlas_;
//      }
//
//    private:
//      shared_ptr<reissner_nordstrom> atlas_;
//    };*/
//
//    const double& mass() const
//    {
//      return m_;
//    }
//
//		const double& charge() const
//		{
//			return q_;
//		}
//
//  private:
//    class interior
//    {
//    public:
//      interior(const double&, const double&);
//      optional<nvector<nvector<double> > > operator()(const nvector<double>&) const;
//
//    private:
//      const double mass;
//			const double charge;
//    };
//
//    class spherical_complement
//    {
//    public:
//      optional<nvector<double> > operator()(const nvector<double>&) const;
//    };
//
//    const double m_;
//		const double q_;
//  };
//}
//
//namespace grwb
//{  
//  using lift::sqr;
//
////  inline reissner_nordstrom::view::view(const shared_ptr<reissner_nordstrom>& _) 
////    : grwb::view(_),
////			atlas_(_)
////  {
//////    stub->set_name("Reissner-Nordstrom");
////  }
//
//  inline reissner_nordstrom::interior::interior(const double& m, const double& q)
//    : mass(m),
//			charge(q)
//  {
//  }
//  
//  inline optional<nvector<nvector<double> > > reissner_nordstrom::interior::operator()(const nvector<double>& x) const
//  {
//    const double r_2(sqr(x[1]));
//    const double potential(1. - 2. * mass / x[1] + sqr(charge) / r_2 );
//
//    if (x[1] <= 0 || x[2] <= 0 || x[2] >= pi || x[3] <= 0. || x[3] >= two_pi)
//      return optional<ntensor_components<2>::type>();
//
//    optional<ntensor_components<2>::type> result(make_ntensor_components<2>(4));
//    
//    (*result)[0][0] = - potential;
//    (*result)[1][1] = 1. / potential;
//    (*result)[2][2] = r_2;
//    (*result)[3][3] = r_2 * sqr(sin(x[2]));
//
//    return result;
//  }
//  
//  inline optional<nvector<double> > reissner_nordstrom::spherical_complement::operator()(const nvector<double>& x) const
//  {
//    const static double three_pi_on_two(pi + half_pi);
//
//    optional<nvector<double> > result;
//    
//    if (x[2] == half_pi && half_pi <= x[3] && x[3] <= three_pi_on_two)
//      return result;
//    
//    nvector<double> y(x);
//    
//    const double sin_theta(sin(x[2]));
//    const double sin_theta_sin_phi(sin_theta * sin(x[3]));
//    
//    y[2] = atan2(sqrt(1. - sqr(sin_theta_sin_phi)), sin_theta_sin_phi);
//    y[3] = atan2(cos(x[2]), - sin_theta * cos(x[3])) + pi;
//
//    if (y[2] == 0 || y[2] == pi || y[3] == 0 || y[3] == two_pi)
//      return result;
//    
//    result.reset(y);
//    return result;
//  }
//  
//  /*inline void reissner_nordstrom::view::draw() const
//  {
//    const double two_m(2. * atlas_->mass());
//
//		if (false)
//		{
//			draw_triangle(make_vector(0., two_m, 0., 0.),
//                  make_vector(0., two_m, pi, 0.),
//                  make_vector(0., two_m, 0., two_pi));
//
//			draw_triangle(make_vector(0., two_m, pi, 0.),
//                  make_vector(0., two_m, 0., two_pi),
//                  make_vector(0., two_m, pi, two_pi));
//		}
//
//    grwb::view::draw();
//  }*/
//
//  inline reissner_nordstrom::reissner_nordstrom(const double& m, const double& q)
//    : atlas(4),
//      m_(m),
//			q_(q)
//  {
//    shared_ptr<chart> alpha_ext(new chart(interior(m, q)));
//    shared_ptr<chart> beta_ext(new chart(interior(m, q)));
//    charts.insert(alpha_ext);
//    charts.insert(beta_ext);
//    shared_ptr<map> phi(new map(spherical_complement()));
//    maps.insert(make_pair(make_pair(alpha_ext, beta_ext), phi));
//    maps.insert(make_pair(make_pair(beta_ext, alpha_ext), phi));
//  }
//}
