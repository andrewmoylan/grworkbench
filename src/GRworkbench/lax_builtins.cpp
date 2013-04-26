#include <vector>
#include <list>
#include <boost/optional.hpp>
#include <lift/constant_iterator.hpp>
#include <lift/differential.hpp>
#include <lift/vector.hpp>
#include "lax/list.hpp"
#include "lax/builtins.hpp"
#include "lax/builtins_helper.hpp"
#include "lax/engine.hpp"
#include "application.hpp"
#include "geodesic.hpp"
#include "kerr.hpp"
#include "lax_builtins.hpp"
#include "lax_function.hpp"
#include "lax_storage.hpp"
#include "linear_distortion.hpp"
#include "metric.hpp"
#include "point.hpp"
#include "spherical_orthonormal.hpp"
#include "tensor.hpp"
#include "to_from_lax.hpp"
#include "view.hpp"
#include "window.hpp"
#include "worldline.hpp"
#include "matrix.hpp"
#include "cached_function.hpp"
#include "parallel_transport.hpp"
#include "connecting_geodesic.hpp"
#include "connecting_null_geodesic.hpp"

#define REGISTER_BUILTIN(NAME) \
	registry.insert(make_pair(#NAME, builtins::NAME()));

#define DEFINE_CACHED_LAX_BUILTIN(TYPE, FUNCTION, NAME, EAGER) \
	DEFINE_LAX_BUILTIN(TYPE, cached_function<TYPE>(FUNCTION), NAME, EAGER)

namespace grwb
{
	namespace lax_integration
	{
		namespace builtins
		{
			using lax_interpreter::lax;
			using std::pair;

			namespace detail
			{
				using lax_interpreter::builtins::I;
				using lax_interpreter::builtins::throw_exception;
				using lax_interpreter::engine;

				const lax scale_camera_speed(const double& s)
				{
					if (s == 0)
						return throw_exception()(string("Cannot scale camera speed by 0"));

					++engine::side_effects();

					camera::turn() *= s;
					return I();
				}

				typedef pair<shared_ptr<window>, shared_ptr<view> > visualisation_window_type;

				const visualisation_window_type visualisation_window(const string& title)
				{
					shared_ptr<view> v(new view);
					shared_ptr<window> w(new window(v, title));
					return make_pair(w, v);
				}

				const lax hide_window(const visualisation_window_type& a)
				{
					++engine::side_effects();
					a.first->hide();
					return I();
				}

				const lax show_window(const visualisation_window_type& a)
				{
					++engine::side_effects();
					a.first->show();
					return I();
				}

				const lax set_use_of_chart_on_window(const visualisation_window_type& wnd, const shared_ptr<chart>& ch, const bool remove)
				{
					++engine::side_effects();
					if (!remove)
					{
						if (wnd.second->charts.find(ch) == wnd.second->charts.end())
						{
							wnd.second->charts.insert(ch);
							wnd.second->dirty();
						}
						else
							throw "That chart is already being visualised on that window.";
					}
					else
					{
						if (wnd.second->charts.erase(ch) == 0)
							throw "That chart is not being visualised on that window.";
						else
							wnd.second->dirty();
					}

					return I();
				}

				const shared_ptr<distortion> linear_distortion(const nvector<nvector<double> >& matrix, const nvector<double>& origin)
				{
					return shared_ptr<distortion>(new grwb::linear_distortion(matrix, origin));
				}
				
				const shared_ptr<distortion> spherical_to_orthonormal_distortion()
				{
					static const shared_ptr<distortion> _(new grwb::spherical_orthonormal());
					return _;
				}

				const lax add_distortion_to_window(const visualisation_window_type& wnd, const shared_ptr<distortion>& dist)
				{
					++engine::side_effects();
					wnd.second->distortions.push_back(dist);
					wnd.second->dirty();
					return I();
				}

				const lax remove_distortion_from_window(const visualisation_window_type& wnd, const shared_ptr<distortion>& dist)
				{
					++engine::side_effects();
					const size_t old(wnd.second->distortions.size());
					wnd.second->distortions.remove(dist);
					if (wnd.second->distortions.size() == old)
						throw "Couldn't remove distortion: it was not attached to that visualisation window";
					else
						wnd.second->dirty();
					return I();
				}

				const lax remove_distortions_from_window(const visualisation_window_type& wnd)
				{
					++engine::side_effects();
					wnd.second->distortions.clear();
					wnd.second->dirty();
					return I();
				}

				const shared_ptr<chart> boyer_lindquist_in_region(const double& m, const double& a, const double& e, const int r)
				{
					return shared_ptr<chart>(new chart(kerr_newman::boyer_lindquist(m, a, e, r)));
				}

				const shared_ptr<inter_chart_map> spherical_revolve()
				{
					return shared_ptr<inter_chart_map>(new inter_chart_map(kerr_newman::revolve()));
				}

				const shared_ptr<chart> kerr_coordinates(const double& m, const double& a, const double& e, const bool ingoing)
				{
					return shared_ptr<chart>(new chart(kerr_newman::kerr_coordinates(m, a, e, ingoing)));
				}

				const shared_ptr<inter_chart_map> boyer_lindquist_to_kerr_coordinates(const double& m, const double& a, const double& e, const int r, const bool is_inv, const bool to_ingo)
				{
					return shared_ptr<inter_chart_map>(new inter_chart_map(kerr_newman::boyer_lindquist_to_kerr_coordinates(m, a, e, r, is_inv, to_ingo)));
				}

				const lax add_map_between_charts(const shared_ptr<chart>& ch1, const shared_ptr<chart>& ch2, const shared_ptr<inter_chart_map>& mp)
				{
					++engine::side_effects();
					inter_chart_maps()[make_pair(ch1, ch2)] = mp;
					return I();
				}

				const lax remove_map_between_charts(const shared_ptr<chart>& ch1, const shared_ptr<chart>& ch2)
				{
					++engine::side_effects();
					inter_chart_maps().erase(inter_chart_maps().find(make_pair(ch1, ch2)));
					return I();
				}

				const optional<shared_ptr<point> > point_on_chart(const shared_ptr<chart>& c, const nvector<double>& x)
				{
					return ((*c)(make_gradient(x))) ? shared_ptr<point>(new point(c, x)) : optional<shared_ptr<point> >();
				}

				const shared_ptr<tangent_vector> tangent_vector_at_point_on_chart(const shared_ptr<point>& p, const shared_ptr<chart>& c, const nvector<double>& v)
				{
					return shared_ptr<tangent_vector>(new tangent_vector(p, c, v));
				}

				const double metric_tensor(const shared_ptr<tangent_vector>& v1, const shared_ptr<tangent_vector>& v2)
				{
					return metric(*v1, *v2);
				}

				const shared_ptr<tangent_flow> geodesic_tangent(const shared_ptr<tangent_vector>& v)
				{
					return shared_ptr<tangent_flow>(new tangent_flow(grwb::geodesic(*v)));
				}

				const shared_ptr<tangent_flow> parallel_transport(const shared_ptr<tangent_flow>& f, const shared_ptr<tangent_vector>& v)
				{
					return shared_ptr<tangent_flow>(new tangent_flow(grwb::parallel_transport(*f, v)));
				}

				const optional<shared_ptr<tangent_flow> > connecting_geodesic_tangent(const shared_ptr<point>& p1, const shared_ptr<point>& p2)
				{
					return connecting_geodesic(p1, p2);
				}

				const optional<pair<double, shared_ptr<tangent_flow> > > connecting_null_geodesic_result(const shared_ptr<point>& p, const shared_ptr<worldline>& cv, const double& guess)
				{
					return grwb::connecting_null_geodesic(p, cv, guess);
				}

				const lax add_chart(const shared_ptr<chart>& c)
				{
					++engine::side_effects();
					charts().insert(c);
					return I();
				}

				const optional<nvector<double> > chart_map(const shared_ptr<chart>& c, const shared_ptr<point>& p)
				{
					return (*p)[c];
				}

				const optional<nvector<double> > tangent_vector_components(const shared_ptr<chart>& c, const shared_ptr<tangent_vector>& t)
				{
					return (*t)[c];
				}

				const lax visualise_curve_on_window(const visualisation_window_type& wnd, const shared_ptr<worldline>& cv, const double& a, const double& b, const int s, const vector<double, 3>& color)
				{
					if (b < a)
						throw "Invalid end point for visualisation: less than start point";

					++engine::side_effects();
					wnd.second->add_spacetime_curve(cv, a, b, s, color);
					return I();
				}

				const lax visualise_coordinate_line_on_window(const visualisation_window_type& wnd, const shared_ptr<coordinate_line>& cv, const double& a, const double& b, const int s, const vector<double, 3>& color)
				{
					++engine::side_effects();
					wnd.second->add_coordinate_curve(cv, a, b, s, color);
					return I();
				}

        const lax visualise_coordinate_surface_on_window(const visualisation_window_type& wnd, const shared_ptr<coordinate_surface>& cv, const vector<double, 2>& x_range, const vector<double, 2>& y_range)
        {
					++engine::side_effects();
          wnd.second->add_coordinate_surface(cv, x_range, y_range);
          return I();
        }

        const lax stop_visualising_coordinate_surface_on_window(const visualisation_window_type& wnd, const shared_ptr<coordinate_surface>& cv)
        {
					++engine::side_effects();
          wnd.second->remove_coordinate_surface(cv);
          return I();
        }

				const lax visualise_surface_on_window(const visualisation_window_type& wnd, const shared_ptr<spacetime_surface>& cv, const vector<double, 2>& x_range, const vector<double, 2>& y_range)
        {
					++engine::side_effects();
          wnd.second->add_spacetime_surface(cv, x_range, y_range);
          return I();
        }

        const lax stop_visualising_surface_on_window(const visualisation_window_type& wnd, const shared_ptr<spacetime_surface>& cv)
        {
					++engine::side_effects();
          wnd.second->remove_spacetime_surface(cv);
          return I();
        }

				const lax stop_visualising_curve_on_window(const visualisation_window_type& wnd, const shared_ptr<worldline>& cv)
				{
					++engine::side_effects();
					wnd.second->remove_spacetime_curve(cv);
					return I();
				}

				const lax stop_visualising_coordinate_line_on_window(const visualisation_window_type& wnd, const shared_ptr<coordinate_line>& cv)
				{
					++engine::side_effects();
					wnd.second->remove_coordinate_curve(cv);
					return I();
				}

				const lax freeze_visualisation_of_curve_on_window(const visualisation_window_type& wnd, const shared_ptr<worldline>& cv)
				{
					++engine::side_effects();
					wnd.second->freeze_spacetime_curve(cv);
					return I();
				}

				const lax freeze_visualisation_of_coordinate_line_on_window(const visualisation_window_type& wnd, const shared_ptr<coordinate_line>& cv)
				{
					++engine::side_effects();
					wnd.second->freeze_coordinate_curve(cv);
					return I();
				}

				const shared_ptr<point> point_of_tangent_vector(const shared_ptr<tangent_vector>& t)
				{
					return t->context();
				}

				const shared_ptr<chart> chart_of_point(const shared_ptr<point>& p)
				{
					return p->valid_chart();
				}

				const lax visualise_vector_on_window(const visualisation_window_type& wnd, const shared_ptr<tangent_vector>& t, const vector<double, 3>& colour)
				{
					++engine::side_effects();
					wnd.second->add_vector(t, colour);
					return I();
				}

				const lax stop_visualising_vector_on_window(const visualisation_window_type& wnd, const shared_ptr<tangent_vector>& t)
				{
					++engine::side_effects();
					wnd.second->remove_vector(t);
					return I();
				}

				const shared_ptr<tangent_vector> multiply_vector(const double& a, const shared_ptr<tangent_vector>& t)
				{
					return shared_ptr<tangent_vector>(new tangent_vector(a * *t));
				}

				const shared_ptr<tangent_vector> add_vectors(const shared_ptr<tangent_vector>& t1, const shared_ptr<tangent_vector>& t2)
				{
					return shared_ptr<tangent_vector>(new tangent_vector(*t1 + *t2));
				}

				const shared_ptr<tangent_vector> orthonormalise_vector_with_respect_to(const shared_ptr<tangent_vector>& t1, const shared_ptr<tangent_vector>& t2)
				{
					return shared_ptr<tangent_vector>(new tangent_vector(orthonormalise(*t1, *t2)));
				}

				const shared_ptr<tangent_vector> normalise_vector(const shared_ptr<tangent_vector>& t)
				{
					return shared_ptr<tangent_vector>(new tangent_vector(normalise(*t)));
				}

        using lift::make_vector;

				const lax faces_of_cube(const int density)
				{
					std::vector<lax> _;
					const double delta(1. / double(density));

					for (double i(-1. + delta / 2.); i <= 1.; i += delta)
						for (double j(-1. + delta / 2.); j <= 1.; j += delta)
						{
							_.push_back(make_vector(1. - delta / 2., i, j));
							_.push_back(make_vector(-1 + delta / 2., i, j));
							_.push_back(make_vector(i, 1. - delta / 2., j));
							_.push_back(make_vector(i, -1. + delta / 2., j));
							_.push_back(make_vector(i, j, 1. - delta / 2.));
							_.push_back(make_vector(i, j, -1. + delta / 2.));
						}

						return lax(lax_interpreter::values::list::make(int(_.size()), _));
				}

				const nvector<nvector<double> > eigenvectors_of_metric(const shared_ptr<chart>& c, const shared_ptr<point>& p)
				{
					nvector<nvector<double> > h(eigen<double>(grwb::value(*(*c)(make_gradient(*(*p)[c])))).vectors);
					for (size_t i(0); i < h.size(); ++i)
					{
						const double m(metric(c, p, h[i]));
						h[i] /= sqrt(abs(m));
						if (m < 0)
							swap(h[0], h[i]);
					}
					return h;
				}

				const lax clear_all_from_window(const visualisation_window_type& wnd)
				{
					++engine::side_effects();
					wnd.second->clear_all();
					return I();
				}

				const lax set_camera_revolution(const visualisation_window_type& wnd, const bool rev)
				{
					++engine::side_effects();
					wnd.second->in_a_spin = rev;
					return I();
				}

				const lax invalidate_window(const visualisation_window_type& wnd)
				{
					++engine::side_effects();
					wnd.second->dirty();
					return I();
				}
			}

			typedef pair<shared_ptr<window>, shared_ptr<view> > visualisation_window_type;

			DEFINE_LAX_BUILTIN(const lax (const double&), detail::scale_camera_speed, scale_camera_speed, false);
			DEFINE_CACHED_LAX_BUILTIN(const visualisation_window_type (const string&), detail::visualisation_window, visualisation_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type &), detail::clear_all_from_window, clear_all_from_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&), detail::hide_window, hide_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&), detail::show_window, show_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const bool), detail::set_camera_revolution, set_camera_revolution, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<chart>&, const bool), detail::set_use_of_chart_on_window, set_use_of_chart_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&), detail::invalidate_window, invalidate_window, false);

			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<worldline>&, const double&, const double&, const int, const vector<double, 3>&), detail::visualise_curve_on_window, visualise_curve_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<worldline>&), detail::stop_visualising_curve_on_window, stop_visualising_curve_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<coordinate_line>&, const double&, const double&, const int, const vector<double, 3>&), detail::visualise_coordinate_line_on_window, visualise_coordinate_line_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<coordinate_line>&), detail::stop_visualising_coordinate_line_on_window, stop_visualising_coordinate_line_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<worldline>&), detail::freeze_visualisation_of_curve_on_window, freeze_visualisation_of_curve_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<coordinate_line>&), detail::freeze_visualisation_of_coordinate_line_on_window, freeze_visualisation_of_coordinate_line_on_window, false);
			
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<tangent_vector>&, const vector<double, 3>&), detail::visualise_vector_on_window, visualise_vector_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<tangent_vector>&), detail::stop_visualising_vector_on_window, stop_visualising_vector_on_window, false);

			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<coordinate_surface>&, const vector<double, 2>&, const vector<double, 2>&), detail::visualise_coordinate_surface_on_window, visualise_coordinate_surface_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<coordinate_surface>&), detail::stop_visualising_coordinate_surface_on_window, stop_visualising_coordinate_surface_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<spacetime_surface>&, const vector<double, 2>&, const vector<double, 2>&), detail::visualise_surface_on_window, visualise_surface_on_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<spacetime_surface>&), detail::stop_visualising_surface_on_window, stop_visualising_surface_on_window, false);

			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<distortion>&), detail::add_distortion_to_window, add_distortion_to_window, false);
      DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&), detail::remove_distortions_from_window, remove_distortions_from_window, false);
			DEFINE_LAX_BUILTIN(const lax (const visualisation_window_type&, const shared_ptr<distortion>&), detail::remove_distortion_from_window, remove_distortion_from_window, false);

			DEFINE_LAX_BUILTIN(const lax (const shared_ptr<chart>&), detail::add_chart, add_chart, false);
			DEFINE_LAX_BUILTIN(const lax (const shared_ptr<chart>&, const shared_ptr<chart>&, const shared_ptr<inter_chart_map>&), detail::add_map_between_charts, add_map_between_charts, false);
			DEFINE_LAX_BUILTIN(const lax (const shared_ptr<chart>&, const shared_ptr<chart>&), detail::remove_map_between_charts, remove_map_between_charts, false);
			
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<distortion> (const nvector<nvector<double> >&, const nvector<double>&), detail::linear_distortion, linear_distortion, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<distortion> (), detail::spherical_to_orthonormal_distortion, spherical_to_orthonormal_distortion, false);

			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<chart> (const double&, const double&, const double&, const int), detail::boyer_lindquist_in_region, boyer_lindquist_in_region, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<chart> (const double&, const double&, const double&, const int), detail::boyer_lindquist_in_region, boyer_lindquist_2_in_region, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<inter_chart_map> (), detail::spherical_revolve, spherical_revolve, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<chart> (const double&, const double&, const double&, const bool), detail::kerr_coordinates, kerr_coordinates, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<chart> (const double&, const double&, const double&, const bool), detail::kerr_coordinates, kerr_coordinates_2, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<inter_chart_map> (const double&, const double&, const double&, const int, const bool, const bool), detail::boyer_lindquist_to_kerr_coordinates, boyer_lindquist_to_kerr_coordinates, false);

			DEFINE_CACHED_LAX_BUILTIN(const optional<shared_ptr<point> > (const shared_ptr<chart>&, const nvector<double>&), detail::point_on_chart, point_on_chart, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_vector> (const shared_ptr<point>&, const shared_ptr<chart>&, const nvector<double>&), detail::tangent_vector_at_point_on_chart, tangent_vector_at_point_on_chart, false);

			DEFINE_LAX_BUILTIN(const double (const shared_ptr<tangent_vector>&, const shared_ptr<tangent_vector>&), detail::metric_tensor, metric_tensor, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_flow> (const shared_ptr<tangent_vector>&), detail::geodesic_tangent, geodesic_tangent, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_flow> (const shared_ptr<tangent_flow>&, const shared_ptr<tangent_vector>&), detail::parallel_transport, parallel_transport, false);

			DEFINE_CACHED_LAX_BUILTIN(const optional<shared_ptr<tangent_flow> > (const shared_ptr<point>&, const shared_ptr<point>&), detail::connecting_geodesic_tangent, connecting_geodesic_tangent, false);
			//DEFINE_LAX_BUILTIN(const optional<pair<double, shared_ptr<tangent_flow> > > (const shared_ptr<point>&, const shared_ptr<worldline>&, const double&), detail::connecting_null_geodesic_result, connecting_null_geodesic_result, false);

			const ::lax_interpreter::lax& connecting_null_geodesic_result() 
			{ 
				static const ::lax_interpreter::lax _(::lax_interpreter::values::function::make<const optional<pair<double, shared_ptr<tangent_flow> > > (const shared_ptr<point>&, const shared_ptr<worldline>&, const double&)>(detail::connecting_null_geodesic_result, "connecting_null_geodesic_result", false)); 
				return _; 
			}

			DEFINE_LAX_BUILTIN(const optional<nvector<double> > (const shared_ptr<chart>&, const shared_ptr<point>&), detail::chart_map, chart_map, false);
			DEFINE_LAX_BUILTIN(const optional<nvector<double> > (const shared_ptr<chart>&, const shared_ptr<tangent_vector>&), detail::tangent_vector_components, tangent_vector_components, false);
			DEFINE_LAX_BUILTIN(const shared_ptr<point> (const shared_ptr<tangent_vector>&), detail::point_of_tangent_vector, point_of_tangent_vector, false);
			DEFINE_LAX_BUILTIN(const shared_ptr<chart> (const shared_ptr<point>&), detail::chart_of_point, chart_of_point, false);

			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_vector> (const double&, const shared_ptr<tangent_vector>&), detail::multiply_vector, multiply_vector, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_vector> (const shared_ptr<tangent_vector>&, const shared_ptr<tangent_vector>&), detail::add_vectors, add_vectors, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_vector> (const shared_ptr<tangent_vector>&), detail::normalise_vector, normalise_vector, false);
			DEFINE_CACHED_LAX_BUILTIN(const shared_ptr<tangent_vector> (const shared_ptr<tangent_vector>&, const shared_ptr<tangent_vector>&), detail::orthonormalise_vector_with_respect_to, orthonormalise_vector_with_respect_to, false);

			DEFINE_LAX_BUILTIN(const lax (const int&), detail::faces_of_cube, faces_of_cube, false);

			DEFINE_LAX_BUILTIN(const nvector<nvector<double> > (const shared_ptr<chart>&, const shared_ptr<point>&), detail::eigenvectors_of_metric, eigenvectors_of_metric, false);

      namespace detail
      {
				void insert_builtins(map<string, lax>& registry)
				{
					REGISTER_BUILTIN(clear_all_from_window);
					REGISTER_BUILTIN(set_camera_revolution);
					REGISTER_BUILTIN(scale_camera_speed);
					REGISTER_BUILTIN(visualisation_window);
					REGISTER_BUILTIN(hide_window);
					REGISTER_BUILTIN(show_window);
					REGISTER_BUILTIN(set_use_of_chart_on_window);
					REGISTER_BUILTIN(invalidate_window);

					REGISTER_BUILTIN(visualise_curve_on_window);
					REGISTER_BUILTIN(stop_visualising_curve_on_window);
					REGISTER_BUILTIN(visualise_coordinate_line_on_window);
					REGISTER_BUILTIN(stop_visualising_coordinate_line_on_window);
          REGISTER_BUILTIN(freeze_visualisation_of_curve_on_window);
					REGISTER_BUILTIN(freeze_visualisation_of_coordinate_line_on_window);

					REGISTER_BUILTIN(add_distortion_to_window);
					REGISTER_BUILTIN(remove_distortions_from_window);
					REGISTER_BUILTIN(remove_distortion_from_window);

					REGISTER_BUILTIN(visualise_vector_on_window);
					REGISTER_BUILTIN(stop_visualising_vector_on_window);

					REGISTER_BUILTIN(visualise_coordinate_surface_on_window);
					REGISTER_BUILTIN(stop_visualising_coordinate_surface_on_window);
					REGISTER_BUILTIN(visualise_surface_on_window);
					REGISTER_BUILTIN(stop_visualising_surface_on_window);

					REGISTER_BUILTIN(boyer_lindquist_in_region);
					REGISTER_BUILTIN(boyer_lindquist_2_in_region);
					REGISTER_BUILTIN(spherical_revolve);
					REGISTER_BUILTIN(kerr_coordinates);
					REGISTER_BUILTIN(kerr_coordinates_2);
					REGISTER_BUILTIN(boyer_lindquist_to_kerr_coordinates);

					REGISTER_BUILTIN(linear_distortion);
					REGISTER_BUILTIN(spherical_to_orthonormal_distortion);

					REGISTER_BUILTIN(point_on_chart);
					REGISTER_BUILTIN(tangent_vector_at_point_on_chart);

					REGISTER_BUILTIN(metric_tensor);

					REGISTER_BUILTIN(geodesic_tangent);
					REGISTER_BUILTIN(parallel_transport);

					REGISTER_BUILTIN(connecting_geodesic_tangent);
					REGISTER_BUILTIN(connecting_null_geodesic_result);

					REGISTER_BUILTIN(add_chart);
					REGISTER_BUILTIN(add_map_between_charts);
					REGISTER_BUILTIN(remove_map_between_charts);

					REGISTER_BUILTIN(chart_map);			
					REGISTER_BUILTIN(tangent_vector_components);
					REGISTER_BUILTIN(point_of_tangent_vector);
					REGISTER_BUILTIN(chart_of_point);

					REGISTER_BUILTIN(add_vectors);
					REGISTER_BUILTIN(multiply_vector);
					REGISTER_BUILTIN(normalise_vector);
					REGISTER_BUILTIN(orthonormalise_vector_with_respect_to);

					REGISTER_BUILTIN(faces_of_cube);
					REGISTER_BUILTIN(eigenvectors_of_metric);
				}

      }

			const map<string, lax>& builtins()
			{
				static optional<map<string, lax> > _;
				if (!_)
				{
					_.reset(map<string, lax>());
					detail::insert_builtins(*_);
				}
				return *_;
			}
		}
	}
}

#undef REGISTER_BUILTIN
