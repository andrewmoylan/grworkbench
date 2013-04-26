#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include "gradient.hpp"
#include "matrix.hpp"
#include "tensor.hpp"

namespace grwb
{

  namespace connection_detail
  {
    class connection_functor
    {
    public:
      template<typename F> connection_functor(const F& f) : _(f)
      {
      }

      optional<nvector<nvector<nvector<double> > > > operator()(const nvector<double>& x) const
      {
        const size_t n(x.size());

        optional<nvector<nvector<differential<double, nvector<double> > > > > y(_(make_gradient(x)));

        if (!y)
          return optional<nvector<nvector<nvector<double> > > >();
				optional<nvector<nvector<double> > > f;

        try
				{
					f = inverse(value(*y));
				}
				catch (singular e)
				{
					return optional<nvector<nvector<nvector<double> > > >();
				}
        nvector<nvector<nvector<double> > > g(gradient(*y));

        nvector<nvector<nvector<double> > > result(n, make_constant_iterator(nvector<nvector<double> >(n, make_constant_iterator(nvector<double>(n)))));
        
        for (size_t a = 0; a < n; ++a)
          for (size_t b = 0; b < n; ++b)
            for (size_t c = 0; c < n; ++c)
              for (size_t d = 0; d < n; ++d)
                result[a][b][c] += .5 * (*f)[d][a] * (g[c][d][b] + g[b][d][c] - g[c][b][d]);
        return result;
      }

    private:
      const function<optional<nvector<nvector<differential<double, nvector<double> > > > >(const nvector<differential<double, nvector<double> > >&)> _;
      connection_functor& operator=(const connection_functor&);
    };
  }
  
  template<typename F> connection_detail::connection_functor connection(const F& metric)
  {
    return connection_detail::connection_functor(metric);
  }

} // grwb

        /*using lift::make_vector;*/

        /*nvector<nvector<nvector<double> > > h(make_vector(
          value(*y) - value(*_(make_gradient(x - nvector<double>(make_vector(1e-5, 0., 0., 0.))))),
          value(*y) - value(*_(make_gradient(x - nvector<double>(make_vector(0., 1e-5, 0., 0.))))),
          value(*y) - value(*_(make_gradient(x - nvector<double>(make_vector(0., 0., 1e-5, 0.))))),
          value(*y) - value(*_(make_gradient(x - nvector<double>(make_vector(0., 0., 0., 1e-5)))))
          ) * 1e5);*/

       //nvector<nvector<nvector<double> > > l(g);

       // for (size_t i(0); i < n; ++i)
       //   for (size_t j(0); j < n; ++j)
       //    for (size_t k(0); k < n; ++k)
       //      l[i][j][k] = g[j][k][i];

        /*using std::cout;
        using std::endl; */

        /*cout << "analytic: " << endl << h << endl;
        cout << "numeric: " << endl << l << endl;
        cout << "difference: " << endl << (h-l) << endl;*/

                //result[a][b][c] += .5 * f[d][a] * (g[b][c][d] + g[c][b][d] - g[d][c][b]);
                //result[a][b][c] += .5 * f[a][d] * (g[c][d][b] + g[b][d][c] - g[d][b][c]);
