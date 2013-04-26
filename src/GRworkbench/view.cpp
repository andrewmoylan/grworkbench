#include <sstream>
#include <boost/shared_ptr.hpp>
#include <lift/cross_product.hpp>
#include <lift/registry.hpp>
#include "application.hpp"
#include "curve.hpp"
#include "object.hpp"
#include "opengl.hpp"
#include "point.hpp"
#include "surface.hpp"
#include "view.hpp"
#include "lax/log_cout.hpp"

namespace grwb
{

	using boost::function;
	using lift::cross_product;

	void view::add_coordinate_curve(const shared_ptr<coordinate_line>& c, const double& a, const double& b, const int s, const vector<double, 3>& col)
	{
		add_curve<nvector<double> >(c, a, b, s, col, coordinate_curves);
	}

	void view::add_spacetime_curve(const shared_ptr<worldline>& c, const double& a, const double& b, const int s, const vector<double, 3>& col)
	{
		add_curve<shared_ptr<point> >(c, a, b, s, col, spacetime_curves);
	}

	void view::add_vector(const shared_ptr<tangent_vector>& t, const vector<double, 3>& colour)
	{
		const map<shared_ptr<tangent_vector>, vector<double, 3> >::const_iterator search(vectors.find(t));
		if (search == vectors.end())
			vectors.insert(make_pair(t, colour));
		else
		{
			vectors[t] = colour;
			lax_interpreter::log_cout << "Updated visualisation parameters." << endl;
		}
		vectors_dirty = true;
	}

	void view::remove_vector(const shared_ptr<tangent_vector>& t)
	{
		const map<shared_ptr<tangent_vector>, vector<double, 3> >::iterator search(vectors.find(t));
		if (search != vectors.end())
		{
			vectors.erase(search);
			vectors_dirty = true;
		}
		else
			throw "Couldn't remove vector visualisation: it was not currently being visualised.";
	}

  void view::add_coordinate_surface(const shared_ptr<coordinate_surface>& cv, const vector<double, 2>& x_range, const vector<double, 2>& y_range)
  {
    const map<shared_ptr<coordinate_surface>, shared_ptr<surface<nvector<double> > > >::const_iterator search(surfaces.find(cv));
    if (search != surfaces.end())
    {
			search->second->update(x_range, y_range);
			lax_interpreter::log_cout << "Updated visualisation parameters." << endl;
    }
    else
      surfaces.insert(make_pair(cv, shared_ptr<surface<nvector<double> > >(new surface<nvector<double> >(*this, cv, x_range, y_range))));
  }

  void view::remove_coordinate_surface(const shared_ptr<coordinate_surface>& cv)
  {
    const map<shared_ptr<coordinate_surface>, shared_ptr<surface<nvector<double> > > >::iterator search(surfaces.find(cv));
		if (search != surfaces.end())
		{
			surfaces.erase(search);
		}
		else
			throw "Couldn't remove surface visualisation: it was not currently being visualised.";
  }

	void view::add_spacetime_surface(const shared_ptr<spacetime_surface>& cv, const vector<double, 2>& x_range, const vector<double, 2>& y_range)
  {
    const map<shared_ptr<spacetime_surface>, shared_ptr<surface<shared_ptr<point> > > >::const_iterator search(spacetime_surfaces.find(cv));
    if (search != spacetime_surfaces.end())
    {
			search->second->update(x_range, y_range);
			lax_interpreter::log_cout << "Updated visualisation parameters." << endl;
    }
    else
      spacetime_surfaces.insert(make_pair(cv, shared_ptr<surface<shared_ptr<point> > >(new surface<shared_ptr<point> >(*this, cv, x_range, y_range))));
  }

  void view::remove_spacetime_surface(const shared_ptr<spacetime_surface>& cv)
  {
    const map<shared_ptr<spacetime_surface>, shared_ptr<surface<shared_ptr<point> > > >::iterator search(spacetime_surfaces.find(cv));
		if (search != spacetime_surfaces.end())
		{
			spacetime_surfaces.erase(search);
		}
		else
			throw "Couldn't remove surface visualisation: it was not currently being visualised.";
  }

	template <class T> void view::add_curve(const shared_ptr<function<optional<T> (const double&)> >& c, const double& a, const double& b, const int s, const vector<double, 3>& col, map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >& list)
	{
		const typename map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >::const_iterator search(list.find(c));
		if (search != list.end())
		{
			search->second->update(a, b, s, col);
			lax_interpreter::log_cout << "Updated visualisation parameters." << endl;
		}
		else
		{
			shared_ptr<curve<T> > cv(new curve<T>(c, a, b, s, col, *this));
			list.insert(make_pair(c, cv));
			cv->invalidate();
		}
	}

	void view::dirty()
	{
		for (list<shared_ptr<object> >::const_iterator i = objects.begin(); i != objects.end(); ++i)
			(*i)->invalidate();

		for (map<shared_ptr<worldline>, shared_ptr<curve<shared_ptr<point> > > >::const_iterator i(spacetime_curves.begin()); i != spacetime_curves.end(); ++i)
			i->second->invalidate();

		for (map<shared_ptr<function<optional<nvector<double> > (const double&)> >, shared_ptr<curve<nvector<double> > > >::const_iterator i(coordinate_curves.begin()); i != coordinate_curves.end(); ++i)
			i->second->invalidate();

    for (map<shared_ptr<coordinate_surface>, shared_ptr<surface<nvector<double> > > >::const_iterator i(surfaces.begin()); i != surfaces.end(); ++i)
			i->second->invalidate();

		for (map<shared_ptr<spacetime_surface>, shared_ptr<surface<shared_ptr<point> > > >::const_iterator i(spacetime_surfaces.begin()); i != spacetime_surfaces.end(); ++i)
			i->second->invalidate();

		vectors_dirty = true;

		drawing_error = false;
	}

	void view::remove_coordinate_curve(const shared_ptr<coordinate_line>& c)
	{
		remove_curve<nvector<double> >(c, coordinate_curves);
	}

	void view::remove_spacetime_curve(const shared_ptr<worldline>& c)
	{
		remove_curve<shared_ptr<point> >(c, spacetime_curves);
	}

	template <class T> void view::remove_curve(const shared_ptr<function<optional<T> (const double&)> >& c, map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >& list)
	{
		const typename map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >::iterator search(list.find(c));
		if (search != list.end())
			list.erase(search);
		else
			throw "Couldn't stop visualising curve: it was not currently being visualised.";
	}

	void view::freeze_coordinate_curve(const shared_ptr<coordinate_line>& c)
	{
		freeze_curve<nvector<double> >(c, coordinate_curves);
	}

	void view::freeze_spacetime_curve(const shared_ptr<worldline>& c)
	{
		freeze_curve<shared_ptr<point> >(c, spacetime_curves);
	}

	template <class T> void view::freeze_curve(const shared_ptr<function<optional<T> (const double&)> >& c, map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >& list)
	{
		const typename map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >::iterator search(list.find(c));
		if (search != list.end())
			search->second->freeze();
		else
			throw "Couldn't freeze visualisation of curve: it was not currently being visualised.";
	}

	void freeze_coordinate_curve(const shared_ptr<worldline>&);

  class grid {
  public:
    vector<double, 3> Origin, Normal, u, v;	// These define the plane onto which the mouse cursor is projected, which asymptote to...
    vector<double, 3> tOrigin, tNormal;		// ... the target (control vectors). Note that u, v are determined from Origin, Normal and a preference for the x-axis
    
    grid();
    
    void Orthogonalise(void);	// Keep sqr
    void Process(const camera&);			// Keep moving
    void Reset(grid &p);		// Reset
    void Reset(void);			// Reset to default values
    
    void NewOrigin(const grwb::vector<double, 3>& v);	// Change current.focus of grid
    void NewNormal(const grwb::vector<double, 3>& v);	// Change normal of grid
    
    void draw(const camera&, const vector<double, 3>&);	// draw the grid
    
    ~grid();
  };
  
  void CoreExit();

  using lift::make_vector;

	void view::process()
	{
		plane_->Process(camera_);
		ProjectCursor();
    if (in_a_spin)
      camera_.zrotate(plane_->Normal);
		camera_.process();
	}

  void view::draw(const vector<size_t, 2>& _) const
  {
    size_ = _;

    glDisable(GL_BLEND);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
		glDisable(GL_ALPHA_TEST);

    camera_.draw(double(size_[0]) / size_[1]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

    draw_objects();

    glDisable(GL_BLEND);

    glDepthMask(false);
    plane_->draw(camera_, Mouse);
  }

  bool view::mouse_move(const vector<int, 2>& v) const
  {
    iMouse = v;
    return true;
  }

  camera::frustrum::frustrum()
    : apex(make_vector(10., 10., 10.))
    , focus(make_vector(0., 0., 0.))
    , up(make_vector(0., 0., 1.))
    , zoom(2.)
  {
  }

	double& camera::turn()
	{
		static double _(0.05);
		return _;
	}

  camera::camera()
    : current()
    , target()
    , slide(.1)
    , snap(false)
  {
  }

	void camera::process()
	{
		Process();
	}

  void camera::draw(const double& aspect) {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(
              aspect * -0.1 / current.zoom,
              aspect *  0.1 / current.zoom,
              -0.1 / current.zoom,
              0.1 / current.zoom,
              0.1,
              10
              );

    double s = 1.0 / abs(current.apex - current.focus);

    glScaled(s, s, s);

    gluLookAt(current.apex[0], current.apex[1], current.apex[2], current.focus[0], current.focus[1], current.focus[2], current.up[0], current.up[1], current.up[2]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void camera::zrotate(const vector<double, 3>& n)
  {
    target.apex += cross_product(target.apex - target.focus, n) * turn() * .1;
  }

  void camera::DriftDown(void) {
    target.apex -= target.up * abs(target.apex - target.focus) * turn();
  }

  void camera::DriftIn(void) {
    target.apex -= (target.apex - target.focus) * turn();
  }

  void camera::DriftLeft(void) 
  {
    target.apex -= cross_product(target.apex - target.focus, target.up * turn());
  }

  void camera::DriftOut(void) {
    target.apex += (target.apex - target.focus) * turn();
  }

  void camera::DriftRight(void) {
    target.apex += cross_product(target.apex - target.focus, target.up * turn());
  }

  void camera::DriftUp(void) {
    target.apex += target.up * abs(target.apex - target.focus) * turn();
  }

  void camera::orthogonalise() 
  {
    target.up = cross_product(target.apex - target.focus, cross_product(target.up, target.apex - target.focus));
    target.up /= abs(target.up);
    current.up = cross_product(current.apex - current.focus, cross_product(current.up, current.apex - current.focus));
    current.up /= abs(current.up);
  }

  void camera::Process(void) {


    if (application::key::state[application::key::left]) { DriftLeft();	}
    if (application::key::state[application::key::right]) { DriftRight();	}
    if (application::key::state[application::key::up]) { DriftUp();	}
    if (application::key::state[application::key::down]) { DriftDown();	}
    if (application::key::state['+'] || application::key::state['=']) { DriftIn();	}
    if (application::key::state['-']) { DriftOut(); }
    if (application::key::state['/']) { ZoomIn(); }
    if (application::key::state['*']) { ZoomOut(); }
    if (application::key::state[application::key::pageup]) { RollRight();	}
    if (application::key::state[application::key::pagedown]) { RollLeft(); }

    if (snap) {
      current.apex = target.apex;
      current.focus = target.focus;
      current.up = target.up;
      current.zoom = target.zoom;
    }
    else {
      current.apex		= (target.apex		* slide) + (current.apex		* (1.0 - slide));
      current.focus	= (target.focus	* slide) + (current.focus	* (1.0 - slide));
      current.up		= (target.up		* slide) + (current.up		* (1.0 - slide));
      current.zoom = (target.zoom * slide) + (current.zoom * (1.0 - slide));
    }
    orthogonalise();
  }

  void camera::ZoomIn(void) {
    current.zoom += turn();
    if(current.zoom > 2) {
      current.zoom = 2;
    }
  }

  void camera::ZoomOut(void) {
    current.zoom -= turn();
    if(current.zoom < turn()) {
      current.zoom = turn();
    }
  }

  void camera::RollRight(void) {
    target.up -= cross_product(target.focus - target.apex, target.up) * turn() / abs(target.focus - target.apex);
  }

  void camera::RollLeft(void) {
    target.up += cross_product(target.focus - target.apex, target.up) * turn() / abs(target.focus - target.apex);
  }

  grid::grid() {
    Reset();

    Normal = tNormal;
    Origin = tOrigin;
  }

  grid::~grid() {
  }

  void grid::NewNormal(const grwb::vector<double, 3>& v) {
    tNormal = v;
    tNormal / abs(tNormal);
  }

  void grid::NewOrigin(const grwb::vector<double, 3>& v) {
    tOrigin = v;
  }

  void grid::Orthogonalise(void) {
    Normal /= abs(Normal);

    if (fabs(Normal[0]) > fabs(Normal[1]))
    {
      if (fabs(Normal[0]) > fabs(Normal[2])) 
      {
        u = cross_product(Normal, make_vector(0., 0., 1.));
      }
      else 
      {
        u = cross_product(Normal, make_vector(0., 1., 0.));
      }
    }
    else {
      // 1 or 2 is the largest
      if(fabs(Normal[1]) > fabs(Normal[2])) {
        // Primary direction: y
        // We mark as an x-z grid
        u = cross_product(Normal, make_vector(0., 0., 1.));
      }
      else {
        // Primary direction z
        // We mark as a x-y grid
        u = cross_product(Normal, make_vector(0., 1., 0.));
      }
    }
    u /= abs(u);
    v = cross_product(u, Normal);
    v /= abs(v);
  }

  void grid::Reset(void) {
    tNormal = make_vector(0., 0., 1.);
    tOrigin = make_vector(0., 0., 0.);
  }

  void grid::Reset(grid &p) {
    *this = p;
  }

  using std::ostringstream;

  void view::ProjectCursor() const
  {
    GLdouble modelMatrix[16], projMatrix[16];
    GLint viewport[4];

    grwb::vector<double, 3> Screen;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    grwb::vector<double, 3> uv;

    gluUnProject(iMouse[0], size_[1] - iMouse[1], 0, modelMatrix, projMatrix, viewport, &Screen[0], &Screen[1], &Screen[2]);

    Mouse =  camera_.current.apex + (Screen - camera_.current.apex) * (inner_product(plane_->Origin - camera_.current.apex, plane_->Normal) / inner_product(Screen - camera_.current.apex, plane_->Normal));

  }

  bool view::key_down(const int c) 
  {
    switch(c) {
    case 'x':
    case 'X':
      plane_->NewNormal(grwb::make_vector(1., 0., 0.));
      break;
    case 'y':
    case 'Y':
      plane_->NewNormal(grwb::make_vector(0., 1., 0.));
      break;
    case 'z':
    case 'Z':
      plane_->NewNormal(grwb::make_vector(0., 0., 1.));
      break;
    case 'p':
    case 'P':
      // Parallel

      if(inner_product(plane_->Normal, camera_.current.apex - camera_.current.focus) >= 0) {
        camera_.target.apex	= plane_->Origin + plane_->Normal * abs(camera_.current.apex - camera_.current.focus);
        camera_.target.focus	= plane_->Origin;
      }
      else {
        camera_.target.apex	= plane_->Origin - plane_->Normal * abs(camera_.current.apex - camera_.current.focus);
        camera_.target.focus	= plane_->Origin;
      }

      if(fabs(inner_product(camera_.current.up, plane_->u)) > fabs(inner_product(camera_.current.up, plane_->v))) {
        if(inner_product(camera_.current.up, plane_->u) > 0) {
          camera_.target.up = plane_->u;
        }
        else {
          camera_.target.up = plane_->u * -1.0;
        }
      }
      else {
        if(inner_product(camera_.current.up, plane_->v) > 0) {
          camera_.target.up = plane_->v;
        }
        else {
          camera_.target.up = plane_->v * -1.0;
        }
      }

      break;
    case 'n':
    case 'N':
      // Normal
      plane_->NewNormal(camera_.current.apex - camera_.current.focus);
      break;
    default:
      break;
    }
    return true;
  }

  bool view::mouse_down(const vector<int, 2>&) 
  {
    plane_->tOrigin = Mouse;
    camera_.target.focus = Mouse;
    return true;
  }

  class projector
  {
  public:
    projector()
    {
      glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
      glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
      glGetIntegerv(GL_VIEWPORT, viewport);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho(viewport[0], viewport[0] + viewport[2], viewport[1], viewport[1] + viewport[3], -10, 0);
    }
    ~projector()
    {
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();    
    }
    optional<vector<double, 3> > operator()(const vector<double, 3>& x) const
    {
      vector<double, 3> y;
      if (gluProject(x[0], x[1], x[2], modelMatrix, projMatrix, viewport, &y[0], &y[1], &y[2]) == GL_TRUE)
        return y;
      else
        return optional<vector<double, 3> >();
    }
  private:
    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint viewport[4];
  };

    using std::cout;
    using std::endl;

  class plane
  {
  public:
    plane(const vector<double, 3>& x, const vector<double, 3>& n)
    {
      _n = n / abs(n);
      _c = inner_product(x, _n);
    }
    double operator()(const vector<double, 3>& x) const
    {
      return inner_product(x, _n) - _c;
    }
  //private:
    vector<double, 3> _n;
           double     _c;    
  };

  class line
  {
  public:
    line(const vector<double, 3>& a, const vector<double, 3>& b) 
      : _a(a), _b(b) 
    {
    }
    vector<double, 3> operator()(const double& t) const
    {
      return _a * (1. - t) + _b * t;
    }
  //private:
    vector<double, 3> _a;
    vector<double, 3> _b;
  };

  optional<vector<double, 3> > intersect_line_plane(const line& l, const plane& p)
  {
    double q = inner_product(l._b - l._a, p._n);
    if (!q)
      return optional<vector<double, 3> >();
    double t = (p._c - inner_product(l._a, p._n)) / q;
    if (t < 0 || t > 1)
      return optional<vector<double, 3> >();
    else
      return l(t);
  }

  optional<vector<double, 3> > intersect_three_planes(const plane& a, const plane& b, const plane& c)
  {
    double q = inner_product(a._n, cross_product(b._n, c._n));
    if (!q)
      return optional<vector<double, 3> >();
    return (a._c * cross_product(b._n, c._n) + b._c * cross_product(c._n, a._n) + c._c * cross_product(a._n, b._n)) / q;

  }

  void grid::draw(const camera& camera_, const vector<double, 3>&) 
  {

    plane g(Origin, Normal);
    

    // construct frustrum

    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    vector<double, 3> vertex[10];
    gluUnProject(viewport[0]              , viewport[1]              , 0, modelMatrix, projMatrix, viewport, &vertex[0][0], &vertex[0][1], &vertex[0][2]);
    gluUnProject(viewport[0]              , viewport[1]              , 1, modelMatrix, projMatrix, viewport, &vertex[1][0], &vertex[1][1], &vertex[1][2]);
    gluUnProject(viewport[0]              , viewport[1] + viewport[3], 0, modelMatrix, projMatrix, viewport, &vertex[2][0], &vertex[2][1], &vertex[2][2]);
    gluUnProject(viewport[0]              , viewport[1] + viewport[3], 1, modelMatrix, projMatrix, viewport, &vertex[3][0], &vertex[3][1], &vertex[3][2]);
    gluUnProject(viewport[0] + viewport[2], viewport[1]              , 0, modelMatrix, projMatrix, viewport, &vertex[4][0], &vertex[4][1], &vertex[4][2]);
    gluUnProject(viewport[0] + viewport[2], viewport[1]              , 1, modelMatrix, projMatrix, viewport, &vertex[5][0], &vertex[5][1], &vertex[5][2]);
    gluUnProject(viewport[0] + viewport[2], viewport[1] + viewport[3], 0, modelMatrix, projMatrix, viewport, &vertex[6][0], &vertex[6][1], &vertex[6][2]);
    gluUnProject(viewport[0] + viewport[2], viewport[1] + viewport[3], 1, modelMatrix, projMatrix, viewport, &vertex[7][0], &vertex[7][1], &vertex[7][2]);

    // construct z-function

    plane zmap(camera_.current.apex, cross_product(vertex[0] - vertex[2], vertex[2] - vertex[4]));

    vector<line, 4> frustrum = make_vector( 
      line(camera_.current.apex, camera_.current.apex + (vertex[0] - camera_.current.apex) / zmap(vertex[0])),
      line(camera_.current.apex, camera_.current.apex + (vertex[2] - camera_.current.apex) / zmap(vertex[2])),
      line(camera_.current.apex, camera_.current.apex + (vertex[4] - camera_.current.apex) / zmap(vertex[4])),
      line(camera_.current.apex, camera_.current.apex + (vertex[6] - camera_.current.apex) / zmap(vertex[6])));


    // the znear and zfar 

    // compute intersection with grid plane

    optional<pair<vector<double, 3>, vector<double, 3> > > extrema;
    optional<pair<double, double> > zextrema;

    for (int i(0); i != 7; ++i)
    {
      for (int j(i + 1); j != 8; ++j)
      {
        optional<vector<double, 3> > x(intersect_line_plane(line(vertex[i], vertex[j]), g));
        if (x)
        {
          if (extrema)
          {
            for (int k(0); k != 3; ++k)
            {
              if (extrema->first[k] > (*x)[k])
                extrema->first[k] = (*x)[k];
              if (extrema->second[k] < (*x)[k])
                extrema->second[k] = (*x)[k];
            }
            if (zextrema->first > zmap(*x))
              zextrema->first = zmap(*x);
            if (zextrema->second < zmap(*x))
              zextrema->second = zmap(*x);
          }
          else
          {
            extrema = make_pair(*x, *x);
            zextrema = make_pair(zmap(*x), zmap(*x));
          }
        }
      }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_LINE_SMOOTH);

    projector proj;

    const double threshold = .02;
    const double brightness = 2.;

    if (extrema) // the grid is visible
    {
      //double near_clipping_z = zmap(vertex[0]);
      //double far_clipping_z = zmap(vertex[1]);

      double near_intersect_z = zextrema->first;
      double far_intersect_z = zextrema->second;

      double scale = pow(10., ceil(log10(zextrema->second / brightness)));

      glBegin(GL_LINES);

      while (scale > near_intersect_z * threshold / brightness)
      {

        double saturation_z = scale * brightness;
        double threshold_z = scale * brightness / threshold;

        if (saturation_z < far_intersect_z && near_intersect_z < threshold_z)
        {

          double z_near = max(saturation_z, near_intersect_z);
          double z_far = min(threshold_z, far_intersect_z);

          vector<double, 3> v[8] = { frustrum[0](z_near), frustrum[1](z_near), frustrum[2](z_near), frustrum[3](z_near), frustrum[0](z_far), frustrum[1](z_far), frustrum[2](z_far), frustrum[3](z_far) };

          optional<pair<vector<double, 3>, vector<double, 3> > > extrema;
          for (int i(0); i != 7; ++i) 
          {
            for (int j(i + 1); j != 8; ++j)
            {
              optional<vector<double, 3> > x(intersect_line_plane(line(v[i], v[j]), g));
              if (x)
              {
                if (extrema)
                {
                  for (int k(0); k != 3; ++k)
                  {
                    if (extrema->first[k] > (*x)[k]) extrema->first[k] = (*x)[k];
                    if (extrema->second[k] < (*x)[k]) extrema->second[k] = (*x)[k];
                  }
                }
                else
                {
                  extrema = make_pair(*x, *x);
                }
              }
            }
          }

          if (extrema)
          {

            double near_alpha = brightness * scale / z_near; // [1, threshold]
            double far_alpha = brightness * scale / z_far; // [1, threshold]
  
            plane near_plane(camera_.current.apex + zmap._n * z_near, zmap._n);
            plane far_plane(camera_.current.apex + zmap._n * z_far, zmap._n);

            for (int i(0); i != 3; ++i)
            {
              vector<double, 3> e;
              e[i] = 1;
              for (double j(scale * ceil(extrema->first[i] / scale)); j < extrema->second[i]; j += scale)
              {
                optional<vector<double, 3> > a(intersect_three_planes(g, near_plane, plane(e * j, e)));
                optional<vector<double, 3> > b(intersect_three_planes(g, far_plane, plane(e * j, e)));
                if (a && b)
                {
                  glColor4d(e[0], e[1], e[2], near_alpha);
                  glVertex3dv(proj(*a)->begin());
                  glColor4d(e[0], e[1], e[2], far_alpha);
                  glVertex3dv(proj(*b)->begin());
                }
              }
            }
          }
        }


        if (saturation_z > near_intersect_z)
        {

          double z_near = near_intersect_z;
          double z_far = min(saturation_z, far_intersect_z);

          vector<double, 3> v[8] = { frustrum[0](z_near), frustrum[1](z_near), frustrum[2](z_near), frustrum[3](z_near),
            frustrum[0](z_far), frustrum[1](z_far), frustrum[2](z_far), frustrum[3](z_far) };

          optional<pair<vector<double, 3>, vector<double, 3> > > extrema;
          for (int i(0); i != 7; ++i) 
          {
            for (int j(i + 1); j != 8; ++j)
            {
              optional<vector<double, 3> > x(intersect_line_plane(line(v[i], v[j]), g));
              if (x)
              {
                if (extrema)
                {
                  for (int k(0); k != 3; ++k)
                  {
                    if (extrema->first[k] > (*x)[k]) extrema->first[k] = (*x)[k];
                    if (extrema->second[k] < (*x)[k]) extrema->second[k] = (*x)[k];
                  }
                }
                else
                {
                  extrema = make_pair(*x, *x);
                }
              }
            }
          }

          plane near_plane(camera_.current.apex + zmap._n * z_near, zmap._n);
          plane far_plane(camera_.current.apex + zmap._n * z_far, zmap._n);

          if (extrema)
          {
            for (int i(0); i != 3; ++i)
            {
              vector<double, 3> e;
              e[i] = 1;
              glColor4d(e[0], e[1], e[2], 1.);
              for (double j(scale * ceil(extrema->first[i] / scale)); j < extrema->second[i]; j += scale)
              {
                optional<vector<double, 3> > a(intersect_three_planes(g, near_plane, plane(e * j, e)));
                optional<vector<double, 3> > b(intersect_three_planes(g, far_plane, plane(e * j, e)));
                if (a && b)
                {
                  glVertex3dv(proj(*a)->begin());
                  glVertex3dv(proj(*b)->begin());
                }
              }
            }
          }
        }


        scale *= .1;

      }
      glEnd();
    }

  }

  void grid::Process(const camera& camera_) 
  {
    double s = camera_.slide;

    Normal = (tNormal * s) + (Normal * (1.0 - s));
    Origin = (tOrigin * s) + (Origin * (1.0 - s));

    Orthogonalise();
  }

  namespace view_detail
  {
    class select
    {
    public:
      select(const function<view*()>& f, shared_ptr<view>& v_) : factory(f), v(v_)
      {
      }

      void operator()() const
      { 
        v = shared_ptr<view>(factory());
      }

    private:
      const function<view*()> factory;
      mutable shared_ptr<view>& v;
      select& operator=(const select&);
    }; // select
  }  

  using boost::optional;
  using lift::vector;

  optional<vector<double, 3> > view::operator()(const shared_ptr<point>& p) const
  {
		for (multiset<shared_ptr<chart> >::const_iterator i(charts.begin()); i != charts.end(); ++i)
		{
			const optional<nvector<double> > coordinates((*p)[*i]);
			if (coordinates)
				return operator()(*coordinates);
		}
		return optional<vector<double, 3> >();
  }

  optional<vector<double, 3> > view::operator()(const nvector<double>& x) const
  {
    optional<nvector<double> > coordinates(x);
    for (list<shared_ptr<distortion> >::const_iterator i = distortions.begin(); i != distortions.end(); ++i)
      if (*i && !(coordinates = value((**i)(make_gradient(*coordinates)))))
        return optional<vector<double, 3> >();

    vector<double, 3> result;
    if (coordinates->size() > 0)
      result[0] = (*coordinates)[0];
    if (coordinates->size() > 1)
      result[1] = (*coordinates)[1];
    if (coordinates->size() > 2)
      result[2] = (*coordinates)[2];

    return result;
  }

  optional<pair<vector<double, 3>, vector<double, 3> > > view::operator()(const shared_ptr<tangent_vector>& p) const
  {
		optional<nvector<double> > v;
		optional<nvector<double> > g;
		for (multiset<shared_ptr<chart> >::const_iterator i(charts.begin()); i != charts.end(); ++i)
		{
			const optional<nvector<double> > coordinates((*p->context())[*i]);
			if (coordinates)
			{
				v = coordinates;
				g = p->operator[](*i);
				break;
			}
		}

		if (!v)
			return optional<pair<vector<double, 3>, vector<double, 3> > >();

    optional<nvector<differential<double, nvector<double> > > > x(make_gradient(*v));
    for (list<shared_ptr<distortion> >::const_iterator i(distortions.begin()); i != distortions.end(); ++i)
      if (!x || /*!*i ||*/ !(x = (**i)(*x)))
        return optional<pair<vector<double, 3>, vector<double, 3> > >();
    
		pair<vector<double, 3>, vector<double, 3> > result;
    if (x->size() > 0)
    {
      result.first[0] = value((*x)[0]);
      result.second[0] = inner_product(*g, gradient((*x)[0]));
    }
    if (x->size() > 1)
    {
      result.first[1] = value((*x)[1]);
      result.second[1] = inner_product(*g, gradient((*x)[1]));
    }
    if (x->size() > 2)
    {
      result.first[2] = value((*x)[2]);
      result.second[2] = inner_product(*g, gradient((*x)[2]));
    }
    return result;
  }

	void view::clear_all()
	{
		charts.clear();
		objects.clear();
		surfaces.clear();
		spacetime_surfaces.clear();
		spacetime_curves.clear();
		coordinate_curves.clear();
		vectors.clear();
		vectors_dirty = true;
		distortions.clear();
		drawing_error = false;
	}

  void view::draw_objects() const
  {
    if (drawing_error)
			return;

		try
		{
			for (list<shared_ptr<object> >::const_iterator i = objects.begin(); i != objects.end(); ++i)
				(*i)->draw();

			for (map<shared_ptr<coordinate_surface>, shared_ptr<surface<nvector<double> > > >::const_iterator i(surfaces.begin()); i != surfaces.end(); ++i)
				i->second->draw();

			for (map<shared_ptr<spacetime_surface>, shared_ptr<surface<shared_ptr<point> > > >::const_iterator i(spacetime_surfaces.begin()); i != spacetime_surfaces.end(); ++i)
				i->second->draw();

			for (map<shared_ptr<worldline>, shared_ptr<curve<shared_ptr<point> > > >::const_iterator i(spacetime_curves.begin()); i != spacetime_curves.end(); ++i)
				i->second->draw();

			for (map<shared_ptr<function<optional<nvector<double> > (const double&)> >, shared_ptr<curve<nvector<double> > > >::const_iterator i(coordinate_curves.begin()); i != coordinate_curves.end(); ++i)
				i->second->draw();

			if (vectors_dirty)
			{
				vectors_gl_list = glGenLists(1);
				glNewList(vectors_gl_list, GL_COMPILE_AND_EXECUTE);
				glBegin(GL_LINES);
				for (map<shared_ptr<tangent_vector>, vector<double, 3> >::const_iterator i(vectors.begin()); i != vectors.end(); ++i)
				{
					optional<pair<vector<double, 3>, vector<double, 3> > > v(operator ()(i->first));
					if (v)
					{
						glColor3dv(i->second.begin());
						glVertex3dv(v->first.begin());
						const vector<double, 3> end(v->first + v->second);
						glVertex3dv(end.begin());	
					}
				}
				glEnd();
				glEndList();
				vectors_dirty = false;
			}
			else
				glCallList(vectors_gl_list);
		}
		catch (string& s)
		{
			lax_interpreter::log_cout << "Exception while drawing objects: " << s << endl;
			drawing_error = true;
		}
		catch (const char* s)
		{
			lax_interpreter::log_cout << "Exception while drawing objects: " << s << endl;
			drawing_error = true;
		}
		catch (...)
		{
			lax_interpreter::log_cout << "Unknown exception while drawing objects" << endl;
			drawing_error = true;
		}
  }

  view::view()
		: size_(make_vector<int>(640, 480)),
		camera_(),
		plane_(new grid()),
		vectors_dirty(true),	
		vectors_gl_list(glGenLists(1)),
		in_a_spin(false), 
		drawing_error(false) 
	{
	}

} // grwb
