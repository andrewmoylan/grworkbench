#pragma once

#include <vector>
#include <iostream>
#include "lax.hpp"
#include "value.hpp"

namespace lax_interpreter
{
	namespace values
	{
		using namespace std;
		using std::vector;

		class list : public value
		{
		public:
			virtual ~list();

			static const lax make(const int, const std::vector<lax>&);

			virtual const lax evaluated_at(const lax&) const;

			virtual void output(ostream&) const;
			virtual const bool is_call() const;

		private:
			list(const int, const std::vector<lax>&);

			const std::vector<lax> vector_;
			const int n_;
		};
	}
}
