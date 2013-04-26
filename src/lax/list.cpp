#include <boost/shared_ptr.hpp>
#include <map>
#include "list.hpp"
#include "builtins.hpp"

namespace lax_interpreter
{
	using namespace std;

	namespace values
	{
		list::~list()
		{
		}

		const lax list::make(const int n, const vector<lax>& v)
		{
			static map<pair<int, vector<lax> >, lax> _;
			const pair<int, vector<lax> > p(n, v);
			const map<pair<int, vector<lax> >, lax>::const_iterator i(_.find(p));
			return i != _.end() ? i->second : _.insert(make_pair(p, lax(shared_ptr<const value>(new list(n, v))))).first->second;
		}

		const lax list::evaluated_at(const lax& a) const
		{
			if (size_t(n_) == vector_.size())
			{
				const int i(a);
				return i >= 0 && i < n_ ? vector_[i].evaluated() : builtins::nil();
			}
			else
			{
				vector<lax> v(vector_);
				v.push_back(a);
				return make(n_, v);
			}
		}

		void list::output(ostream& o) const
		{
			o << "list " << n_;
			for (vector<lax>::const_iterator i(vector_.begin()); i != vector_.end(); ++i)
			{
				const bool parentheses_required(i->is_call());
				o << " " << (parentheses_required ? "(" : "") << *i << (parentheses_required ? ")" : "");
			}
		}

		list::list(const int n, const vector<lax>& v)
			: vector_(v),
			n_(n)
		{
		}

		const bool list::is_call() const
		{
			return true;
		}
	}
}
