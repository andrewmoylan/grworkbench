#pragma once

#include <boost/optional.hpp>
#include <map>
#include <tuple>

namespace grwb
{

	template <class T> struct flatten
	{
		typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
	};

#define flat_type(T) \
	typename flatten<T>::type

	template <class F> class cached_function;

	template <class R> class cached_function<R ()>
	{
	public:
		typedef function<R ()> function_type;
		typedef typename function_type::result_type result_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()() const
		{
			if (!cache_)
				cache_ = f_();
			return *cache_;
		}

	private:
		const function_type f_;
		mutable optional<result_type> cache_;
	};

	template <class R, class A> class cached_function<R (A)>
	{
	public:
		typedef function<R (A)> function_type;
		typedef flat_type(A) argument_type;
		typedef typename function_type::result_type result_type;
		typedef map<argument_type, result_type> cache_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()(const argument_type& a) const
		{
			const typename cache_type::const_iterator search(cache_.find(a));
			return search != cache_.end() ? search->second : cache_.insert(make_pair(a, f_(a))).first->second;
		}

	private:
		const function_type f_;
		mutable cache_type cache_;
	};

	template <class R, class A, class B> class cached_function<R (A, B)>
	{
	public:
		typedef function<R (A, B)> function_type;
		typedef flat_type(A) arg1_type;
		typedef flat_type(B) arg2_type;
		typedef std::tuple<arg1_type, arg2_type> tuple_type;
		typedef typename function_type::result_type result_type;
		typedef map<tuple_type, result_type> cache_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()(const arg1_type& a1, const arg2_type& a2) const
		{
			const tuple_type a(a1, a2);
			const typename cache_type::const_iterator search(cache_.find(a));
			return search != cache_.end() ? search->second : cache_.insert(make_pair(a, f_(a1, a2))).first->second;
		}

	private:
		const function_type f_;
		mutable cache_type cache_;
	};

	template <class R, class A, class B, class C> class cached_function<R (A, B, C)>
	{
	public:
		typedef function<R (A, B, C)> function_type;
		typedef flat_type(A) arg1_type;
		typedef flat_type(B) arg2_type;
		typedef flat_type(C) arg3_type;
		typedef std::tuple<arg1_type, arg2_type, arg3_type> tuple_type;
		typedef typename function_type::result_type result_type;
		typedef map<tuple_type, result_type> cache_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()(const arg1_type& a1, const arg2_type& a2, const arg3_type& a3) const
		{
			const tuple_type a(a1, a2, a3);
			const typename cache_type::const_iterator search(cache_.find(a));
			return search != cache_.end() ? search->second : cache_.insert(make_pair(a, f_(a1, a2, a3))).first->second;
		}

	private:
		const function_type f_;
		mutable cache_type cache_;
	};

	template <class R, class A, class B, class C, class D> class cached_function<R (A, B, C, D)>
	{
	public:
		typedef function<R (A, B, C, D)> function_type;
		typedef flat_type(A) arg1_type;
		typedef flat_type(B) arg2_type;
		typedef flat_type(C) arg3_type;
		typedef flat_type(D) arg4_type;
		typedef std::tuple<arg1_type, arg2_type, arg3_type, arg4_type> tuple_type;
		typedef typename function_type::result_type result_type;
		typedef map<tuple_type, result_type> cache_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()(const arg1_type& a1, const arg2_type& a2, const arg3_type& a3, const arg4_type& a4) const
		{
			const tuple_type a(a1, a2, a3, a4);
			const typename cache_type::const_iterator search(cache_.find(a));
			return search != cache_.end() ? search->second : cache_.insert(make_pair(a, f_(a1, a2, a3, a4))).first->second;
		}

	private:
		const function_type f_;
		mutable cache_type cache_;
	};

	template <class R, class A, class B, class C, class D, class E> class cached_function<R (A, B, C, D, E)>
	{
	public:
		typedef function<R (A, B, C, D, E)> function_type;
		typedef flat_type(A) arg1_type;
		typedef flat_type(B) arg2_type;
		typedef flat_type(C) arg3_type;
		typedef flat_type(D) arg4_type;
		typedef flat_type(E) arg5_type;
		typedef std::tuple<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> tuple_type;
		typedef typename function_type::result_type result_type;
		typedef map<tuple_type, result_type> cache_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()(const arg1_type& a1, const arg2_type& a2, const arg3_type& a3, const arg4_type& a4, const arg5_type& a5) const
		{
			const tuple_type a(a1, a2, a3, a4, a5);
			const typename cache_type::const_iterator search(cache_.find(a));
			return search != cache_.end() ? search->second : cache_.insert(make_pair(a, f_(a1, a2, a3, a4, a5))).first->second;
		}

	private:
		const function_type f_;
		mutable cache_type cache_;
	};

	template <class R, class A, class B, class C, class D, class E, class F> class cached_function<R (A, B, C, D, E, F)>
	{
	public:
		typedef function<R (A, B, C, D, E, F)> function_type;
		typedef flat_type(A) arg1_type;
		typedef flat_type(B) arg2_type;
		typedef flat_type(C) arg3_type;
		typedef flat_type(D) arg4_type;
		typedef flat_type(E) arg5_type;
		typedef flat_type(F) arg6_type;
		typedef std::tuple<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> tuple_type;
		typedef typename function_type::result_type result_type;
		typedef map<tuple_type, result_type> cache_type;

		cached_function(const function_type& f)
			: f_(f)
		{
		}

		result_type operator ()(const arg1_type& a1, const arg2_type& a2, const arg3_type& a3, const arg4_type& a4, const arg5_type& a5, const arg6_type& a6) const
		{
			const tuple_type a(a1, a2, a3, a4, a5, a6);
			const typename cache_type::const_iterator search(cache_.find(a));
			return search != cache_.end() ? search->second : cache_.insert(make_pair(a, f_(a1, a2, a3, a4, a5, a6))).first->second;
		}

	private:
		const function_type f_;
		mutable cache_type cache_;
	};
}

#undef flat_type
