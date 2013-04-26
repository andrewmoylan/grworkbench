#include <cmath>
#include <map>
#include <string>
#include <boost/optional.hpp>
#include "builtins.hpp"
#include "list.hpp"
#include "parse.hpp"
#include "engine.hpp"
#include "builtins_helper.hpp"
#include "list.hpp"
#include "log_cout.hpp"
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#define STD_FUNCTION_WRAPPER(NAME) \
class NAME##_wrapper \
{ \
public: \
	const double operator()(const double& x) const \
{ \
	return ::std::NAME(x); \
} \
}

#define STD_FUNCTION_WRAPPER_2(NAME) \
class NAME##_wrapper \
{ \
public: \
	const double operator()(const double& x, const double& y) const \
{ \
	return ::std::NAME(x, y); \
} \
}

#define REGISTER_BUILTIN(NAME) \
	registry.insert(make_pair(#NAME, builtins::NAME()));

namespace lax_interpreter
{
	using namespace std;
	using namespace boost;

	namespace builtins
	{
		namespace detail
		{
			void insert_builtins(map<string, lax>& registry)
			{
				REGISTER_BUILTIN(quotation_mark);

				REGISTER_BUILTIN(nil);

				REGISTER_BUILTIN(list);

				REGISTER_BUILTIN(get_dynamic_variable);
				REGISTER_BUILTIN(set_dynamic_variable);

				REGISTER_BUILTIN(left);
				REGISTER_BUILTIN(right);
				REGISTER_BUILTIN(head);
				REGISTER_BUILTIN(tail);

				REGISTER_BUILTIN(sine);
				REGISTER_BUILTIN(absolute_value);
				REGISTER_BUILTIN(arc_cosine);
				REGISTER_BUILTIN(arc_sine);
				REGISTER_BUILTIN(arc_tangent);
				REGISTER_BUILTIN(ceiling);
				REGISTER_BUILTIN(cosine);
				REGISTER_BUILTIN(hyperbolic_cosine);
				REGISTER_BUILTIN(exponential);
				REGISTER_BUILTIN(floor);
				REGISTER_BUILTIN(logarithm);
				REGISTER_BUILTIN(hyperbolic_sine);
				REGISTER_BUILTIN(square_root);
				REGISTER_BUILTIN(tangent);
				REGISTER_BUILTIN(hyperbolic_tangent);

				REGISTER_BUILTIN(power);
				REGISTER_BUILTIN(arc_tangent_2);

				REGISTER_BUILTIN(equal);
				REGISTER_BUILTIN(inequal);
				REGISTER_BUILTIN(identical);
				REGISTER_BUILTIN(nonidentical);

				REGISTER_BUILTIN(if_then_else);

				REGISTER_BUILTIN(I);
				REGISTER_BUILTIN(K);
				REGISTER_BUILTIN(S);

				REGISTER_BUILTIN(Kn);
				REGISTER_BUILTIN(Sn);
				REGISTER_BUILTIN(K_after_n);

				REGISTER_BUILTIN(add);
				REGISTER_BUILTIN(subtract);
				REGISTER_BUILTIN(multiply);
				REGISTER_BUILTIN(divide);

				REGISTER_BUILTIN(negate);

				REGISTER_BUILTIN(logical_not);
				REGISTER_BUILTIN(logical_and);
				REGISTER_BUILTIN(logical_or);

				REGISTER_BUILTIN(less_than);
				REGISTER_BUILTIN(greater_than);
				REGISTER_BUILTIN(less_than_or_equal);
				REGISTER_BUILTIN(greater_than_or_equal);

				REGISTER_BUILTIN(is_boolean);
				REGISTER_BUILTIN(is_character);
				REGISTER_BUILTIN(is_integer);
				REGISTER_BUILTIN(is_real);

				REGISTER_BUILTIN(show);
				REGISTER_BUILTIN(show_string);
				REGISTER_BUILTIN(showed);
				REGISTER_BUILTIN(showed_string);

				REGISTER_BUILTIN(include_file);

				REGISTER_BUILTIN(throw_exception);

				REGISTER_BUILTIN(evaluated);

				REGISTER_BUILTIN(numeric_range);

				REGISTER_BUILTIN(current_directory);
				REGISTER_BUILTIN(change_directory);
				REGISTER_BUILTIN(system_command);
				REGISTER_BUILTIN(is_microsoft_windows);

				REGISTER_BUILTIN(set_logging);
			}

			const string quotation_mark()
			{
				return "\"";
			}

			const lax set_logging(const bool b)
			{
				enable_log = b;
				return I();
			}

			const bool identical(const lax& a, const lax& b)
			{
				return a == b;
			}

			const bool nonidentical(const lax& a, const lax& b)
			{
				return a != b;
			}

			const bool equality(const lax& a, const lax& b)
			{
				return a == b || a.evaluated() == b.evaluated();
			}

			const bool inequality(const lax& a, const lax& b)
			{
				return a != b && a.evaluated() != b.evaluated();
			}

			const lax if_then_else(const bool x)
			{
				return x ? K() : K()(I());
			}

			const lax s_combinator(const lax& a, const lax& b, const lax& c)
			{
				return a(c)(b(c));
			}

			const lax sn(const int n)
			{
				return n == 0 ? I() : (n == 1 ? S() : S()(K()(S()))(S()(K()(Sn()(n - 1)))));
			}

			const lax k_combinator(const lax& a, const lax&)
			{
				return a;
			}

			const lax kn(const int n, const lax& a)
			{
				return n == 0 ? a : K()(n == 1 ? a : kn(n - 1, a));
			}

			const lax identity_combinator(const lax& a)
			{
				return a;
			}

			const lax nil(const lax&)
			{
				return builtins::nil();
			}

			const lax k_after_n(const int n)
			{
				return n == 0 ? K() : S()(K()(K_after_n()(n -  1)));
			}

			const lax left(const lax& a)
			{
				return a.evaluated().get_left();
			}

			const lax right(const lax& a)
			{
				return a.evaluated().get_right();
			}

			const lax head(const lax& a)
			{
				return left(left(a)) == S() ? right(left(a)) : K()(a);
			}

			const lax tail(const lax& a)
			{
				return left(left(a)) == S() ? right(a) : I();
			}

			void display(const lax& a)
			{
				log_cout << a.evaluated();
			}

			const lax include_file(const string& file_name)
			{
				++engine::side_effects();
				parser::include_file(file_name, *engine::current_context());
				return I();
			}

			void display_string(const string& a)
			{
				log_cout << a;
			}

			const lax showed(const lax& a)
			{
				++engine::side_effects();
				display(a);
				return a;
			}

			const lax show(const lax& a)
			{
				++engine::side_effects();
				display(a);
				return I();
			}

			const lax showed_string(const string& a)
			{
				++engine::side_effects();
				display_string(a);
				return a;
			}

			const lax show_string(const string& a)
			{
				++engine::side_effects();
				display_string(a);
				return I();
			}

			const lax evaluated(const lax& a)
			{
				return a.evaluated();
			}

			const lax throw_exception(const string& a)
			{
				throw a;
			}

			const lax list(const int n)
			{
				return values::list::make(n, vector<lax>());
			}

			const lax numeric_range(const double& inc, const double& a, const double& b)
			{
				if (a + inc == a || b - inc == b)
					throw "Zero increment in numeric_range";
				vector<lax> _;
				for (double i(a); a < b && i <= b || a > b && i >= a; i += inc)
					_.push_back(i);
				return lax(values::list::make(int(_.size()), _));
			}

			map<string, lax> dynamic_variables;

			const lax set_dynamic_variable(const string& var, const lax& val)
			{
				++engine::side_effects();
				dynamic_variables.erase(var);
				dynamic_variables.insert(make_pair(var, val));
				return I();
			}

			const lax get_dynamic_variable(const string& var)
			{
				++engine::side_effects();
				map<string, lax>::const_iterator search(dynamic_variables.find(var));
				if (search == dynamic_variables.end())
					throw "Invalid call to get_dynamic_variable: dynamic variable was not defined";

				return search->second;
			}

			const string current_directory()
			{
#ifndef _WIN32
#define _MAX_PATH PATH_MAX
#endif
				++engine::side_effects();
				char buffer[_MAX_PATH];
				getcwd(buffer, _MAX_PATH);
				return string(buffer);
			}

			const lax change_directory(const string& d)
			{
				++engine::side_effects();
				chdir(d.c_str());
				return I();
			}

			const lax system_command(const string& d)
			{
				++engine::side_effects();
				system(d.c_str());
				return I();
			}

			const bool is_microsoft_windows()
			{
#ifdef _WIN32
				return true;
#else
				return false;
#endif
			}

			STD_FUNCTION_WRAPPER(sin);
			STD_FUNCTION_WRAPPER(abs);
			STD_FUNCTION_WRAPPER(acos);
			STD_FUNCTION_WRAPPER(asin);
			STD_FUNCTION_WRAPPER(atan);
			STD_FUNCTION_WRAPPER(ceil);
			STD_FUNCTION_WRAPPER(cos);
			STD_FUNCTION_WRAPPER(cosh);
			STD_FUNCTION_WRAPPER(exp);
			STD_FUNCTION_WRAPPER(floor);
			STD_FUNCTION_WRAPPER(log);
			STD_FUNCTION_WRAPPER(sinh);
			STD_FUNCTION_WRAPPER(sqrt);
			STD_FUNCTION_WRAPPER(tan);
			STD_FUNCTION_WRAPPER(tanh);

			STD_FUNCTION_WRAPPER_2(pow);
			STD_FUNCTION_WRAPPER_2(atan2);
		}

		DEFINE_LAX_BUILTIN(const string (), detail::quotation_mark, quotation_mark, false);

		DEFINE_LAX_BUILTIN(const lax (const bool), detail::set_logging, set_logging, false);

		DEFINE_LAX_BUILTIN(const lax (const int), detail::list, list, false);

		DEFINE_LAX_BUILTIN(const lax (const string&), detail::get_dynamic_variable, get_dynamic_variable, false);
		DEFINE_LAX_BUILTIN(const lax (const string&, const lax), detail::set_dynamic_variable, set_dynamic_variable, false);

		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::nil, nil, true);

		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::identity_combinator, I, true);
		DEFINE_LAX_BUILTIN(const lax (const lax&, const lax&), detail::k_combinator, K, true);
		DEFINE_LAX_BUILTIN(const lax (const lax&, const lax&, const lax&), detail::s_combinator, S, true);

		DEFINE_LAX_BUILTIN(const lax (const int, const lax&), detail::kn, Kn, true);
		DEFINE_LAX_BUILTIN(const lax (const int), detail::sn, Sn, true);
		DEFINE_LAX_BUILTIN(const lax (const int), detail::k_after_n, K_after_n, true);

		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::left, left, true);
		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::right, right, true);
		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::head, head, true);
		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::tail, tail, true);

		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::evaluated, evaluated, true);

		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::show, show, false);
		DEFINE_LAX_BUILTIN(const lax (const string&), detail::show_string, show_string, false);
		DEFINE_LAX_BUILTIN(const lax (const lax&), detail::showed, showed, false);
		DEFINE_LAX_BUILTIN(const lax (const string&), detail::showed_string, showed_string, false);
		
		DEFINE_LAX_BUILTIN(const lax (const string&), detail::include_file, include_file, false);

		DEFINE_LAX_BUILTIN(const lax (const string&), detail::throw_exception, throw_exception, false);
		
		DEFINE_LAX_BUILTIN(const double (const double&), detail::sin_wrapper(), sine, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::abs_wrapper(), absolute_value, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::acos_wrapper(), arc_cosine, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::asin_wrapper(), arc_sine, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::atan_wrapper(), arc_tangent, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::ceil_wrapper(), ceiling, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::cos_wrapper(), cosine, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::cosh_wrapper(), hyperbolic_cosine, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::exp_wrapper(), exponential, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::floor_wrapper(), floor, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::log_wrapper(), logarithm, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::sinh_wrapper(), hyperbolic_sine, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::sqrt_wrapper(), square_root, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::tan_wrapper(), tangent, false);
		DEFINE_LAX_BUILTIN(const double (const double&), detail::tanh_wrapper(), hyperbolic_tangent, false);

		DEFINE_LAX_BUILTIN(const double (const double&, const double&), detail::pow_wrapper(), power, false);
		DEFINE_LAX_BUILTIN(const double (const double&, const double&), detail::atan2_wrapper(), arc_tangent_2, false);

		DEFINE_LAX_BUILTIN(const bool (const lax&, const lax&), detail::equality, equal, false);
		DEFINE_LAX_BUILTIN(const bool (const lax&, const lax&), detail::inequality, inequal, false);
		DEFINE_LAX_BUILTIN(const bool (const lax&, const lax&), detail::identical, identical, false);
		DEFINE_LAX_BUILTIN(const bool (const lax&, const lax&), detail::nonidentical, nonidentical, false);

		DEFINE_LAX_BUILTIN(const lax (const bool), detail::if_then_else, if_then_else, false);

		DEFINE_LAX_BUILTIN(const double (const double&, const double&), plus<double>(), add, false);
		DEFINE_LAX_BUILTIN(const double (const double&, const double&), minus<double>(), subtract, false);
		DEFINE_LAX_BUILTIN(const double (const double&, const double&), multiplies<double>(), multiply, false);
		DEFINE_LAX_BUILTIN(const double (const double&, const double&), divides<double>(), divide, false);

		DEFINE_LAX_BUILTIN(const double (const double&), std::negate<double>(), negate, false);

		DEFINE_LAX_BUILTIN(const bool (const bool), std::logical_not<bool>(), logical_not, false);
		DEFINE_LAX_BUILTIN(const bool (const bool, const bool), std::logical_and<bool>(), logical_and, false);
		DEFINE_LAX_BUILTIN(const bool (const bool, const bool), std::logical_or<bool>(), logical_or, false);

		DEFINE_LAX_BUILTIN(const bool (const double&, const double&), std::less<double>(), less_than, false);
		DEFINE_LAX_BUILTIN(const bool (const double&, const double&), greater<double>(), greater_than, false);
		DEFINE_LAX_BUILTIN(const bool (const double&, const double&), less_equal<double>(), less_than_or_equal, false);
		DEFINE_LAX_BUILTIN(const bool (const double&, const double&), greater_equal<double>(), greater_than_or_equal, false);

		DEFINE_LAX_BUILTIN(const bool (const lax&), is_lax<bool>, is_boolean, false);
		DEFINE_LAX_BUILTIN(const bool (const lax&), is_lax<char>, is_character, false);
		DEFINE_LAX_BUILTIN(const bool (const lax&), is_lax<int>, is_integer, false);
		DEFINE_LAX_BUILTIN(const bool (const lax&), is_lax<double>, is_real, false);

		DEFINE_LAX_BUILTIN(const lax (const double&, const double&, const double&), detail::numeric_range, numeric_range, false);

		DEFINE_LAX_BUILTIN(const string (), detail::current_directory, current_directory, false);
		DEFINE_LAX_BUILTIN(const lax (const string&), detail::change_directory, change_directory, false);
		DEFINE_LAX_BUILTIN(const lax (const string&), detail::system_command, system_command, false);
		DEFINE_LAX_BUILTIN(const bool (), detail::is_microsoft_windows, is_microsoft_windows, false);

		const map<string, lax>& builtins()
		{
			static optional<map<string, lax> > _;
			if (!_)
			{
				_.reset(map<string, lax>());
				detail::insert_builtins(*_);
			}
			return *_;
		}
	}
}

#undef STD_FUNCTION_WRAPPER
#undef STD_FUNCTION_WRAPPER_2
#undef REGISTER_BUILTIN
