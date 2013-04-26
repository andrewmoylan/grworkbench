#include <iostream>
#include "lax.hpp"
#include "basic_types.hpp"
#include "engine.hpp"
#include "list.hpp"
#include "builtins.hpp"

namespace lax_interpreter
{

  const shared_ptr<const values::value> to_value<int>::operator()(const int& i) const
  {
    return values::integer::make(i);
  }

  const shared_ptr<const values::value> to_value<bool>::operator()(const bool& i) const
  {
    return values::boolean::make(i);
  }

  const shared_ptr<const values::value> to_value<double>::operator()(const double& i) const
  {
    return values::real::make(i);
  }

  const shared_ptr<const values::value> to_value<char>::operator()(const char& i) const
  {
    return values::character::make(i);
  }

  const shared_ptr<const values::value> to_value<string>::operator()(const string& s) const
  {
    return values::list::make(int(s.size()), vector<lax>(s.begin(), s.end())).value();
  }

  const double from_value<double>::operator()(const shared_ptr<const values::value>& p) const
  {
    return p->get_real();
  }

  const int from_value<int>::operator()(const shared_ptr<const values::value>& p) const
  {
    return p->get_integer();
  }

  const bool from_value<bool>::operator()(const shared_ptr<const values::value>& p) const
  {
    return p->get_boolean();
  }

  const char from_value<char>::operator()(const shared_ptr<const values::value>& p) const
  {
    return p->get_character();
  }

  const string from_value<string>::operator()(const shared_ptr<const values::value>& p) const
  {
		string s;
		for (int i(0); p->evaluated_at(i).evaluated() != builtins::nil(); ++i)
			s.push_back(char(p->evaluated_at(i).evaluated()));
		return s;
	}

  const lax lax::evaluated() const
  {
		engine::begin_call();
    const lax r(_->evaluated());
		engine::end_call();
		return r;
  }

	const lax lax::evaluated_at(const lax& a) const
  {
		engine::begin_call();
    const lax r(_->evaluated_at(a));
		engine::end_call();
		return r;
  }

	const lax lax::operator()(const lax& l) const
	{
		engine::begin_call();
    const lax r(_->bound_to(l));
		engine::end_call();
		return r;
	}

	const bool lax::is_call() const
	{
		return _->is_call();
	}

  void lax::swap(lax& l)
  {
    std::swap(_, l._);
  }

  const bool _is_lax_bool(const lax& l)
  {
    return l.value()->is_boolean();
  }

  const bool _is_lax_char(const lax& l)
  {
    return l.value()->is_character();
  }
  
  const bool _is_lax_int(const lax& l)
  {
    return l.value()->is_integer();
  }

  const bool _is_lax_double(const lax& l)
  {
    return l.value()->is_real();
  }

  template<> const bool is_lax<bool>(const lax& l)
  {
    return _is_lax_bool(l);
  }

  template<> const bool is_lax<char>(const lax& l)
  {
    return _is_lax_char(l);
  }

  template<> const bool is_lax<int>(const lax& l)
  {
    return _is_lax_int(l);
  }

  template<> const bool is_lax<double>(const lax& l)
  {
    return _is_lax_double(l);
  }

  ostream& operator<<(ostream& o, const lax& l)
  {
    l.value()->output(o);
    return o;
  }

  const bool operator==(const lax& left, const lax& right)
  {
		return left.value() == right.value();
  }

  const bool operator!=(const lax& left, const lax& right)
  {
    return left.value() != right.value();
  }

  const bool operator <(const lax& left, const lax& right)
  {
    return left.value() < right.value();
  }

	const lax lax::get_left() const
	{
		return _->get_left();
	}

	const lax lax::get_right() const
	{
		return _->get_right();
	}

	const shared_ptr<const values::value>& lax::value() const
	{
		return _;
	}
}
