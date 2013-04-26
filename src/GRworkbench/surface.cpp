//#include "surface.hpp"
//#include "view.hpp"
//#include <lift/cross_product.hpp>
//#include "application.hpp"
//
//
//namespace grwb
//{
//	/*template <class T> surface::surface(view& v, const shared_ptr<surface_type>& p, const vector<double, 2>& a, const vector<double, 2>& b)
//		: object(v), _(p), x_range(a), y_range(b), f(*p, v), error(false)
//	{
//		f.set_owner(*this);
//		init();
//		application::push(idle(*this));
//	}*/
//
//	/*void surface::init()
//	{
//		{
//			const vector<double, 2> l(make_vector(x_range[0], y_range[0]));
//			const vector<double, 2> m(make_vector(x_range[0], y_range[1]));
//			const vector<double, 2> n(make_vector(x_range[1], y_range[0]));
//			_intermediate.push_back(triangle(l, m, n, f(l), f(m), f(n)));
//		}
//		{
//			const vector<double, 2> l(make_vector(x_range[0], y_range[1]));
//			const vector<double, 2> m(make_vector(x_range[1], y_range[1]));
//			const vector<double, 2> n(make_vector(x_range[1], y_range[0]));
//			_intermediate.push_back(triangle(l, m, n, f(l), f(m), f(n)));
//		}
//	}*/
//
//	//void surface::invalidate() 
//	//{
//	//	if (_intermediate.empty())
//	//		application::push(idle(*this));
//	//	_intermediate.clear();
//	//	_finalised.clear();
//	//	display_lists.clear();
//	//	init();
//	//}
//
//
//	//void surface::draw() const
// //   {
//	//		// guessery
//	//		glDisable(GL_BLEND);
//	//		glDepthMask(false);
//	//		glEnable(GL_BLEND);
//	//		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//
//	//		display_lists.draw();
//
//	//		const GLint new_list((_finalised.size() >= 100 || (_intermediate.empty() && _finalised.size() >= 1)) ? glGenLists(1) : 0);
//	//		if (new_list)
//	//			glNewList(new_list, GL_COMPILE_AND_EXECUTE);
//
//	//		glBegin(GL_TRIANGLES);
//
//	//		for (list<triangle>::const_iterator i(_finalised.begin()); i != _finalised.end(); ++i)
// //     {
//	//			vector<double, 3> n(cross_product(i->_fb - i->_fa, i->_fc - i->_fa));
//	//			n /= -2.*abs(n);
//	//			n += make_vector(.5, .5, .5);
//	//			glColor4d(n[0], n[1], n[2], 0.25);
//
// //       glVertex3dv(i->_fa.begin());
// //       glVertex3dv(i->_fb.begin());
//	//			glVertex3dv(i->_fc.begin());
// //     }
//	//		glEnd();
//	//		if (new_list)
//	//		{
//	//			glEndList();
//	//			display_lists.push_back(new_list);
//	//			_finalised.clear();
//	//		}
//
//	//		glBegin(GL_TRIANGLES);
// //     for (list<triangle>::const_iterator j(_intermediate.begin()); j != _intermediate.end(); ++j)
// //     {
//	//			vector<double, 3> n(cross_product(j->_fb - j->_fa, j->_fc - j->_fa));
//	//			n /= -2.*abs(n);
//	//			n += make_vector(.5, .5, .5);
//	//			glColor4d(n[0], n[1], n[2], 0.25);
//
// //       glVertex3dv(j->_fa.begin());
// //       glVertex3dv(j->_fb.begin());
//	//			glVertex3dv(j->_fc.begin());
// //     }
// //     glEnd();
//	//	}
//
//	/*vector<double, 3> surface::view_surface::operator ()(const vector<double, 2>& x)
//	{
//		optional<T> temp(_s(x));
//		if (!temp)
//		{
//			_->error = true;
//			throw "Surfaces not defined for all parameter values for which it was to be visualised";
//		}
//
//		return *(_v(*temp));
//	}*/
//}
