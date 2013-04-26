#include <iostream>
#include <map>
#include "call.hpp"
#include "engine.hpp"

namespace lax_interpreter
{
  namespace values
  {
    call::call(const lax& left, const lax& right)
			: _left(left),
      _right(right),
      _cache()
    {
    }

    call::~call()
    {
    }

		map<pair<lax, lax>, shared_ptr<const value> > call::calls;

    const lax call::make(const lax& left, const lax& right)
    {
			map<pair<lax, lax>, shared_ptr<const value> >::const_iterator i(calls.find(make_pair(left, right)));
			return lax(i != calls.end() ? i->second : calls.insert(make_pair(make_pair(left, right), shared_ptr<const value>(new call(left, right)))).first->second);
    }

		void call::invalidate_cache() const
		{
			_cache = optional<const lax>();
		}

		void call::invalidate_caches()
		{
			for (map<pair<lax, lax>, shared_ptr<const value> >::iterator i(calls.begin()); i != calls.end(); ++i)
				lax(i->second).dynamic_value<values::call>()->invalidate_cache();
		}

    const lax call::cached_result() const
    {
			if (_cache)
				return *_cache;
			else
			{
				const int a(engine::side_effects());
				const lax result(_left.evaluated().evaluated_at(_right));
				if (engine::side_effects() == a)
					_cache = result;
				return result;
			}
			/*else if (_left.cached() && _right.cached())
				return *(_cache = optional<const lax>(_left.evaluated_at(_right)));
			else
				return _left.evaluated_at(_right);*/
    }

    const lax call::evaluated() const
    {
			return cached_result().evaluated();
    }

    const lax call::evaluated_at(const lax& a) const
    {
			return evaluated().evaluated_at(a);
    }

		const bool call::is_call() const
		{
			return true;
		}

    void call::output(ostream& o) const
    {
			const bool parentheses_required(_right.is_call());
			o << _left << ' ' << (parentheses_required ? "(" : "") << _right << (parentheses_required ? ")" : "");
    }

    const bool call::is_boolean() const
    {
      return is_lax<bool>(cached_result());
    }

    const bool call::is_character() const
    {
      return is_lax<char>(cached_result());
    }

    const bool call::is_integer() const
    {
      return is_lax<int>(cached_result());
    }

    const bool call::is_real() const
    {
      return is_lax<double>(cached_result());
    }

    const bool call::get_boolean() const
    {
      return cached_result();
    }

    const char call::get_character() const
    {
      return cached_result();
    }

    const int call::get_integer() const
    {
      return cached_result();
    }

    const double call::get_real() const
    {
      return cached_result();
    }

		const lax call::get_left() const
		{
			return _left;
		}

		const lax call::get_right() const
		{
			return _right;
		}
  }
}

