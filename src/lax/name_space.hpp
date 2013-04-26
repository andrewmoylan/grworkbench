#pragma once

#include <string>
#include <map>
#include "lax.hpp"

namespace lax_interpreter
{
	namespace parser
	{
		using namespace std;

		class name_space
		{
		public:
			name_space();
			virtual ~name_space();

			virtual const lax operator[](const string&) const = 0;
			virtual const int order() const = 0;
			virtual void insert_indirect(const string&);
			virtual void insert_builtin(const pair<string, lax>&);
			void insert_map(const map<string, lax>&);

		protected:
			map<string, lax> names_;
		};

		class top_level_name_space : public name_space
		{
		public:
			top_level_name_space(const bool);
			virtual ~top_level_name_space();

			virtual void insert_indirect(const string&);
			virtual void insert_builtin(const pair<string, lax>&);
			virtual const lax operator[](const string&) const;
			virtual const int order() const;
		};

		class child_name_space : public name_space
		{
		public:
			explicit child_name_space(const name_space&);
			virtual ~child_name_space();

			virtual const lax operator[](const string&) const;
			virtual const int order() const;

		private:
			const name_space& parent_;
      child_name_space& operator=(const child_name_space&);
		};
	}
}
