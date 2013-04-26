#pragma once

#include <sstream>
#include <boost/shared_ptr.hpp>
#include <lift/constant_iterator.hpp>
#include "distortion.hpp"

namespace grwb
{

  using lift::make_constant_iterator;

  class linear_distortion : public distortion
  {
  public:
    linear_distortion(const nvector<nvector<double> >& m, const nvector<double>& o)
      : matrix(m),
			origin(o),
        dimension_in(m.size()),
        dimension_out(m[0].size())//,
//        matrix_stub(boost::shared_ptr<CStub>())
    {
//      dimensionality_stub = stub()->Submenu("Dimensionality");
//      dimensionality_stub->Slider("Dimensions in", (int*)&ui_dimension_in, optional<double>(1.), optional<double>(10.), 1);
//      dimensionality_stub->Slider("Dimensions out", (int*)&ui_dimension_out, optional<double>(1.), optional<double>(10.), 1);
//      dimensionality_stub->Action("Set", set_menu_callback_(this));
//      stub()->Promote(stub()->Seek("Dimensionality"));
//      generate_ui();
    }

    //const std::string& name() const
    //{
    //  static std::string name_("Linear Distortion");
    //  return name_;  
    //}

    virtual optional<nvector<differential<double, nvector<double> > > > operator()(const nvector<differential<double, nvector<double> > >& p) const
    {
      if (p.size() != dimension_in)
        return optional<nvector<differential<double, nvector<double> > > >();
      nvector<differential<double, nvector<double> > > r(dimension_out, make_constant_iterator(differential<double, nvector<double> >(0., nvector<double>(gradient(p[0]).size()))));
      for (size_t i(0); i != dimension_in; ++i)
        for (size_t j(0); j != dimension_out; ++j)
          r[j] += matrix[i][j] * (p[i] - origin[i]);
      return r;
    }

    /*
    virtual optional<nvector<double> > operator()(const nvector<double>& p) const 
    {
      optional<nvector<double> > r;
      if (p.size() == dimension_in)
      {
        r = optional<nvector<double> >(nvector<double>(dimension_out));
        nvector<double> p2(p - origin);

				for (std::size_t i = 0; i < dimension_in; ++i)
					(*r) += matrix[i] * p2[i];
       }
      return r;
    }
    
    virtual optional<nvector<double> > undistort(const nvector<double>& p) const
    {
      optional<nvector<double> > r;
      if (p.size() == dimension_out)
      {
        r = optional<nvector<double> >(nvector<double>(dimension_in));
        
        for (size_t i = 0; i < dimension_out; i++) {
          double ip = inner_product((matrix)[i], (matrix)[i]);
          if (ip > 0)
            *r += (matrix)[i] * (p[i] / ip);
        }
        *r += origin;
      }
      return r;
    }
    */
    
    //void generate_ui() 
    //{
    //  dimension_in = ui_dimension_in;
    //  dimension_out = ui_dimension_out;
    //  
    //  if (matrix_stub)
    //    stub()->Remove(stub()->Seek("Matrix"));
    //  
    //  if (matrix)
    //    delete matrix;
    //  
    //  matrix_stub = stub()->Submenu("Matrix");
    //  stub()->Promote(stub()->Seek("Matrix"), 6);
    //  stub()->Promote(stub()->Seek("Matrix"), -1);
    //  
    //  matrix = new nvector<nvector<double> >(dimension_out + 1, make_constant_iterator(nvector<double>(dimension_in)));
    //  
    //  for (size_t i = 0; i < dimension_out + 1; ++i) {
    //    if (i < dimension_in && i < dimension_out)
    //      (*matrix)[i][i] = 1;
    //    
    //    std::stringstream s;
    //    s << static_cast<unsigned int>(i);
    //    
    //    std::string caption;
    //    if (i == dimension_out)
    //      caption = "Origin";
    //    else
    //      caption = "Basis " + s.str();
    //    
    //    boost::shared_ptr<CStub> basis_stub = matrix_stub->Submenu(caption);
    //    
    //    for (size_t j  = 0; j < dimension_in; ++j)
    //    {
    //      std::stringstream s2;
    //      s2 << static_cast<unsigned int>(j);
    //      basis_stub->Slider("Component " + s2.str(), &(*matrix)[i][j], optional<double>(), optional<double>(), 1);
    //    }
    //  }
    //}

    const nvector<nvector<double> > matrix;
		const nvector<double> origin;
    
  private:

    //class set_menu_callback_
    //{
    //public:
    //  set_menu_callback_(linear_distortion* p)
    //    : _(p)
    //  {
    //  }
    //  
    //  void operator()() const
    //  {
    //    _->generate_ui();
    //  }
    //  
    //private:
    //  linear_distortion* _;
    //};
    
    //    static void set_menu_callback(void* dummy, CMenu* menu);

    std::size_t dimension_in;
    std::size_t dimension_out;
    /*std::size_t ui_dimension_in;
    std::size_t ui_dimension_out;*/
    
//    boost::shared_ptr<CStub> dimensionality_stub;
//    boost::shared_ptr<CStub> matrix_stub;
    
    linear_distortion& operator=(const linear_distortion&);
  }; // linear_distortion
  
} // grwb

