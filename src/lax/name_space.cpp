#include <iostream>
#include "name_space.hpp"
#include "builtins.hpp"
#include "definition.hpp"
#include "call.hpp"
#include "log_cout.hpp"

namespace lax_interpreter
{
	namespace parser
	{
		using namespace std;

		name_space::name_space()
			: names_()
		{
		}

		name_space::~name_space()
		{
		}

		void name_space::insert_indirect(const string& s)
		{
			const map<string, lax>::const_iterator i(names_.find(s));
			if (i == names_.end())
				names_.insert(make_pair(s, lax(values::expanding_definition::make(s))));
			else
				throw string("Overdefined identifier: ") + s + ".";
		}

		void name_space::insert_builtin(const pair<string, lax>& p)
		{
			const map<string, lax>::const_iterator i(names_.find(p.first));
			if (i == names_.end())
				names_.insert(p);
			else
				throw string("Overdefined identifier: ") + p.first + ".";
		}

		void name_space::insert_map(const map<string, lax>& m)
		{
			for (map<string, lax>::const_iterator i(m.begin()); i != m.end(); ++i)
				insert_builtin(*i);
		}

		top_level_name_space::top_level_name_space(const bool include_builtins)
			: name_space()
		{
			if (include_builtins)
			{
				log_cout << "Loading builtins ..." << endl;
				insert_map(builtins::builtins());
			}
		}

		top_level_name_space::~top_level_name_space()
		{
		}

		const lax top_level_name_space::operator[](const string& s) const
		{
			const map<string, lax>::const_iterator i(names_.find(s));
			if (i != names_.end())
				return i->second;

			throw string("Undefined identifier: ") + s + ".";
		}

		void top_level_name_space::insert_indirect(const string& s)
		{
			const map<string, lax>::iterator i(names_.find(s));
			if (i != names_.end())
			{
				log_cout << string("Warning, redefined ") + s << endl;
				i->second.dynamic_value<values::definition>()->unassign();
				values::call::invalidate_caches();
			}
			else
			{
				log_cout << "Defined " + s << endl;
				names_.insert(make_pair(s, lax(values::definition::make(s))));
			}
		}

		void top_level_name_space::insert_builtin(const pair<string, lax>& p)
		{
			const map<string, lax>::const_iterator i(names_.find(p.first));
			if (i == names_.end())
			{
				names_.insert(p);
				log_cout << "Defined builtin " + p.first << endl;
			}
			else
				throw string("Overdefined builtin: ") + p.first + ".";
		}

		const int top_level_name_space::order() const
		{
			return 0;
		}

		child_name_space::child_name_space(const name_space& parent)
			: parent_(parent),
			name_space()
		{
			names_.insert(make_pair("?", builtins::Kn()(order() - 1)(builtins::I())));
		}

		child_name_space::~child_name_space()
		{
		}

		const int child_name_space::order() const
		{
			return parent_.order() + 1;
		}

		const lax child_name_space::operator[](const string& s) const
		{
			const map<string, lax>::const_iterator i(names_.find(s));
			if (i != names_.end())
				return i->second;

			const string resolved_name(s[0] == ':' ? string(s.begin() + 1, s.end()) : s);
			return builtins::K_after_n()(order() - 1)(parent_[resolved_name]);
		}
	}
}
