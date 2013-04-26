#pragma once

#include <iostream>
#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include "lax.hpp"
#include "value.hpp"

namespace lax_interpreter
{
	namespace values
	{
		namespace function
		{
			namespace call_function_with_values_detail
			{
				template <class F, size_t N> struct implementation;

				template <class F> struct implementation<F, 0>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>&) const
					{
						return f();
					}
				};

				template <class F> struct implementation<F, 1>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0]);
					}
				};

				template <class F> struct implementation<F, 2>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0], v[1]);
					}
				};

				template <class F> struct implementation<F, 3>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0], v[1], v[2]);
					}
				};

				template <class F> struct implementation<F, 4>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0], v[1], v[2], v[3]);
					}
				};

				template <class F> struct implementation<F, 5>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0], v[1], v[2], v[3], v[4]);
					}
				};

				template <class F> struct implementation<F, 6>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0], v[1], v[2], v[3], v[4], v[5]);
					}
				};

				template <class F> struct implementation<F, 7>
				{
					typename boost::function_traits<F>::result_type operator()(const boost::function<F>& f, const std::vector<lax>& v) const
					{
						return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6]);
					}
				};
			}
      
 			template <class F> typename boost::function_traits<F>::result_type call_function_with_values(const boost::function<F>& f, const std::vector<lax>& v)
			{
				return call_function_with_values_detail::implementation<F, boost::function_traits<F>::arity>()(f, v);
			}

			template <class F, size_t N> class implementation : public value
			{
			public:
				static const lax make(const boost::function<F>& f, const string& s, const bool eager, const std::vector<lax>& v)
				{
					static map<pair<string, vector<lax> >, lax> _;
					const pair<string, vector<lax> > p(s, v);
					const map<pair<string, vector<lax> >, lax>::const_iterator i(_.find(p));
					return i != _.end() ? i->second : _.insert(make_pair(p, lax(shared_ptr<const value>(new implementation<F, N>(f, s, eager, v))))).first->second;
				}

				virtual const lax evaluated() const
				{
					return N == 0 ? lax(call_function_with_values(_, _v)) : value::evaluated();
				}

				virtual ~implementation()
				{
				}

				virtual const lax evaluated_at(const lax& a) const
				{
					if (N == 0)
						return evaluated().evaluated_at(a);

					std::vector<lax> v(_v);
					v.push_back(a);
								
					return ((v.size() == N) ? lax(call_function_with_values(_, v)) : make(_, _name, _eager, v));
				}

				virtual void output(ostream& o) const
				{
					o << _name;
					for (vector<lax>::const_iterator i(_v.begin()); i != _v.end(); ++i)
					{
						const bool parentheses_required(i->is_call());
						o << ' ' << (parentheses_required ? "(" : "") << *i << (parentheses_required ? ")" : "");
					}
				}

				virtual const lax bound_to(const lax& a) const
				{
					return _eager ? evaluated_at(a) : value::bound_to(a);
				}

				virtual const lax get_left() const
				{
					if (_v.size() == 0)
						return value::get_left();

					std::vector<lax> v(_v);
					v.pop_back();
					return make(_, _name, _eager, v);
				}

				virtual const lax get_right() const
				{
					return _v.size() != 0 ? _v.back() : value::get_right();
				}

				virtual const bool is_call() const
				{
					return _v.size() != 0;
				}

			private:
				explicit implementation(const boost::function<F>& f, const string& s, const bool eager, const std::vector<lax>& v)
					: _(f),
					_name(s),
					_eager(eager),
					_v(v)
				{
				}

				implementation();
				implementation(const implementation&);
				implementation& operator=(const implementation&);

				const bool _eager;
				const boost::function<F> _;
				const string _name;
				const std::vector<lax> _v;
			};

			template <typename F> static const lax make(const boost::function<F>& f, const string& s, const bool eager/*, const bool cached*/)
			{
				return implementation<F, boost::function_traits<F>::arity>::make(f, s, eager, std::vector<lax>() /*false*//*, cached*/);
			}
		}
	}
}
