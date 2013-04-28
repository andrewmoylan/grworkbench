#pragma once

#include <vector>
#include  <lift/vector.hpp>
#include "nvector.hpp"
#include "lax_storage.hpp"
#include "lax_function.hpp"
#include "lax/builtins.hpp"
#include "worldline.hpp"
#include "window.hpp"
#include "view.hpp"
#include "lax_builtins.hpp"
#include "typed_lax_interface.hpp"

namespace lax_interpreter
{
  using namespace grwb;
  using namespace boost;
  using lax_integration::typed_lax_interface;

#define ENABLE_STORAGE(TYPE, NAME) \
  template<> class to_value<TYPE > \
  { \
  public: \
    const std::shared_ptr<const values::value> operator()(const TYPE& p) const \
    { \
		return ::grwb::lax_integration::values::storage<TYPE >::make(p, NAME).value(); \
    } \
  }; \
\
  template<> class from_value<TYPE > \
  { \
  public: \
    const TYPE operator()(const std::shared_ptr<const values::value>& p) const \
    { \
		const optional<TYPE > o(::grwb::lax_integration::values::storage<TYPE >::extract_from(lax(p))); \
			if (o) \
				return *o; \
				\
			throw string("Could not extract stored ") + NAME; \
    } \
  }

#define ENABLE_FUNCTION_STORAGE(BOOST_FUNCTION_TYPE, NAME) \
  template<> class to_value<std::shared_ptr<BOOST_FUNCTION_TYPE > > \
  { \
  public: \
    const std::shared_ptr<const values::value> operator()(const std::shared_ptr<BOOST_FUNCTION_TYPE >& p) const \
    { \
		return ::grwb::lax_integration::values::lax_function<std::shared_ptr<BOOST_FUNCTION_TYPE > >::make(p, NAME).value(); \
    } \
  }; \
\
  template<> class from_value<std::shared_ptr<BOOST_FUNCTION_TYPE > > \
  { \
  public: \
    const std::shared_ptr<BOOST_FUNCTION_TYPE > operator()(const std::shared_ptr<const values::value>& p) const \
    { \
		const optional<std::shared_ptr<BOOST_FUNCTION_TYPE > > o(::grwb::lax_integration::values::lax_function<std::shared_ptr<BOOST_FUNCTION_TYPE > >::extract_from(lax(p))); \
			if (o) \
				return *o; \
			else \
				return typed_lax_interface<BOOST_FUNCTION_TYPE >::make(lax(p)); \
    } \
  }

  typedef pair<std::shared_ptr<window>, std::shared_ptr<view> > visualisation_window_type;

  ENABLE_STORAGE(std::shared_ptr<point>, "[point]");
  ENABLE_STORAGE(visualisation_window_type, "[visualisation window]");
  ENABLE_STORAGE(std::shared_ptr<distortion>, "[distortion]");
  ENABLE_STORAGE(std::shared_ptr<tangent_vector>, "[tangent vector]");

  ENABLE_FUNCTION_STORAGE(worldline, "[worldline]");
	ENABLE_FUNCTION_STORAGE(tangent_flow, "[real -> tangent vector]");
  ENABLE_FUNCTION_STORAGE(coordinate_line, "[coordinate line]");
  ENABLE_FUNCTION_STORAGE(chart, "[chart]");
  ENABLE_FUNCTION_STORAGE(inter_chart_map, "[inter-chart map]");
  ENABLE_FUNCTION_STORAGE(coordinate_surface, "[coordinate surface]");
  ENABLE_FUNCTION_STORAGE(spacetime_surface, "[spacetime surface]");

  template <class T> class from_value<optional<T> >
  {
  public:
    const optional<T> operator()(const std::shared_ptr<const values::value>& p) const
    {
      lax l(p);
      return (l == builtins::nil()) ? optional<T>() : optional<T>(l.operator const T());
    }
  };

  template <class T> class to_value<optional<T> >
  {
  public:
    const std::shared_ptr<const values::value> operator()(const optional<T>& o) const
    {
      return (!o) ? builtins::nil().value() : to_value<T>()(*o);
    }
  };

  template <class T> class from_value<nvector<T> >
  {
  public:
    const nvector<T> operator()(const std::shared_ptr<const values::value>& p) const
    {
      lax l(p);
      std::vector<T> _;
      for (int i(0); l(i).evaluated() != lax_interpreter::builtins::nil(); ++i)
        _.push_back(l(i).operator const T());
      return nvector<T>(_.size(), _.begin());
    }
  };

  template <class T> class to_value<nvector<T> >
  {
  public:
    const std::shared_ptr<const values::value> operator()(const nvector<T>& v) const
    {
      lax l(lax_interpreter::builtins::list()(int(v.size())));
      for (size_t i(0); i < v.size(); ++i)
        l = l(lax(to_value<T>()(v[i])));
      return l.value();
    }
  };
  
  template <size_t N, class T> class from_value< ::lift::vector<T, N> >
  {
  public:
    const ::lift::vector<T, N> operator()(const std::shared_ptr<const values::value>& p) const
    {
      lax l(p);
      std::vector<T> _;
      for (int i(0); l(i).evaluated() != lax_interpreter::builtins::nil(); ++i)
        _.push_back(l(i).operator const T());
      return ::lift::vector<T, N>(_.begin());
    }
  };

  template <size_t N, class T> class to_value< lift::vector<T, N> >
  {
  public:
    const std::shared_ptr<const values::value> operator()(const lift::vector<T, N>& v) const
    {
      lax l(lax_interpreter::builtins::list()(int(N)));
      for (size_t i(0); i < N; ++i)
        l = l(lax(to_value<T>()(v[i])));
      return l.value();
    }
  };

	template <class S, class T> class from_value<std::pair<S, T> >
	{
	public:
		const std::pair<S, T> operator()(const std::shared_ptr<const values::value>& p) const
		{
			lax l(p);
			return std::pair<S, T>(l(0), l(1));
		}
	};

	template <class S, class T> class to_value<std::pair<S, T> >
	{
	public:
		const std::shared_ptr<const values::value> operator()(const std::pair<S, T>& p) const
		{
			return values::list::make(2, std::vector<lax>())(p.first)(p.second).value();
		}
	};

  template<class T, class U> class to_value<differential<T, U> >
  {
  public:
    const std::shared_ptr<const values::value> operator()(const differential<T, U>& x) const
    {
      return lax_interpreter::builtins::list()(2)(lax(to_value<T>()(value(x))))(lax(to_value<U>()(gradient(x)))).value();
    }
  };

  template<class T, class U> class from_value<differential<T, U> >
  {
  public:
    const differential<T, U> operator()(const std::shared_ptr<const values::value>& v) const
    {
      lax x(v);
      return differential<T, U>(x(0).operator const T(), x(1).operator const U());
    }
  };
}
