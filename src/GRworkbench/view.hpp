#pragma once

#include <list>
#include <map>
#include <string>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <lift/vector.hpp>
#include "atlas.hpp"
#include "distortion.hpp"
#include "pane.hpp"
#include "tensor.hpp"
#include "worldline.hpp"

namespace grwb
{

	using std::list;
	using std::map;
	using lift::vector;
	using boost::optional;
  using boost::shared_ptr;
	using boost::weak_ptr;

  template<typename T> class nvector;

  class object;
	template <class T> class curve;
	template <class T> class surface;
	class point;
  class workspace;

  class camera
  {
    friend class grid;
		friend class view;
  public:
    camera();
    void draw(const double&);
		void process();

		static double& turn();
  private:
    void orthogonalise();
    struct frustrum
    {
      frustrum();
      vector<double, 3> apex;
      vector<double, 3> focus;
      vector<double, 3> up;
      double zoom;
    };
    frustrum current;
    frustrum target;

    double slide;
    bool snap;

    void Process(void);

    void DriftLeft(void);
    void DriftRight(void);
    void DriftUp(void);
    void DriftDown(void);
    void DriftIn(void);
    void DriftOut(void);
    void ZoomIn(void);
    void ZoomOut(void);
    void RollLeft(void);
    void RollRight(void);
    void zrotate(const vector<double, 3>&);
  };

  class grid;

	class view : public pane
  {
  public: 

    bool in_a_spin;
		mutable bool drawing_error;

    view();
    virtual ~view() {}
    
    optional<vector<double, 3> > operator()(const shared_ptr<point> &) const;
    optional<vector<double, 3> > operator()(const nvector<double>&) const;
    optional<pair<vector<double, 3>, vector<double, 3> > > operator()(const shared_ptr<tangent_vector>&) const;
		template <class T> optional<vector<double, 3> > operator()(const optional<T> & p) const
		{
			return p ? operator()(*p) : optional<vector<double, 3> >();
		}

		virtual bool mouse_move(const vector<signed int, 2>&) const;
    virtual bool mouse_down(const vector<signed int, 2>&);
    virtual bool key_down(const int);
    virtual void draw(const vector<unsigned int, 2>&) const;
		virtual void process();

		void draw_objects() const;

		void clear_all();    

    list<shared_ptr<object> > objects;

		void freeze_spacetime_curve(const shared_ptr<worldline>&);
		void freeze_coordinate_curve(const shared_ptr<coordinate_line>&);

		void add_coordinate_curve(const shared_ptr<coordinate_line>&, const double&, const double&, const int, const vector<double, 3>&);
		void add_spacetime_curve(const shared_ptr<worldline>&, const double&, const double&, const int, const vector<double, 3>&);
    void add_coordinate_surface(const shared_ptr<coordinate_surface>&, const vector<double, 2>&, const vector<double, 2>&);
		void add_spacetime_surface(const shared_ptr<spacetime_surface>&, const vector<double, 2>&, const vector<double, 2>&);
		void add_vector(const shared_ptr<tangent_vector>&, const vector<double, 3>&);

		void remove_coordinate_curve(const shared_ptr<coordinate_line>&);
		void remove_spacetime_curve(const shared_ptr<worldline>&);
    void remove_coordinate_surface(const shared_ptr<coordinate_surface>&);
		void remove_spacetime_surface(const shared_ptr<spacetime_surface>&);
		void remove_vector(const shared_ptr<tangent_vector>&);
    
    list<shared_ptr<distortion> > distortions;
    multiset<shared_ptr<chart> > charts;

		void dirty();
     
  private:

		mutable bool vectors_dirty;
		mutable GLint vectors_gl_list;

		template <class T> void add_curve(const shared_ptr<function<optional<T> (const double&)> >&, const double&, const double&, const int, const vector<double, 3>&, map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >&);
		template <class T> void remove_curve(const shared_ptr<function<optional<T> (const double&)> >&, map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >&);
		template <class T> void freeze_curve(const shared_ptr<function<optional<T> (const double&)> >&, map<shared_ptr<function<optional<T> (const double&)> >, shared_ptr<curve<T> > >&);

		map<shared_ptr<worldline>, shared_ptr<curve<shared_ptr<point> > > > spacetime_curves;
		map<shared_ptr<function<optional<nvector<double> > (const double&)> >, shared_ptr<curve<nvector<double> > > > coordinate_curves;
    map<shared_ptr<coordinate_surface>, shared_ptr<surface<nvector<double> > > > surfaces;
		map<shared_ptr<spacetime_surface>, shared_ptr<surface<shared_ptr<point> > > > spacetime_surfaces;
		map<shared_ptr<tangent_vector>, vector<double, 3> > vectors;

		void ProjectCursor() const;
    mutable vector<unsigned int, 2> size_;
		mutable camera camera_;
		shared_ptr<grid> plane_;
		mutable vector<double, 3> Mouse;
    mutable vector<signed int, 2> iMouse;

    view(const view&);
    view& operator=(const view&);

  };

}

