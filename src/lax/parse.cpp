#include <map>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <boost/spirit.hpp>
#include "parse.hpp"
#include "definition.hpp"
#include "lax.hpp"
#include "builtins.hpp"
#include "engine.hpp"
#include "log_cout.hpp"

namespace lax_interpreter
{
	namespace parser
	{
		using namespace std;
		using namespace boost;

		struct identifier_grammar : public spirit::grammar<identifier_grammar>
		{
			template<typename T> struct definition
			{    
				definition(const identifier_grammar&)
					: result(spirit::lexeme_d[(spirit::alpha_p || '_') >> *(spirit::alnum_p || '_')])
				{
				}

				const spirit::rule<T>& start()
				{
					return result;
				}

			private:
				const spirit::rule<T> result;
			};
		} identifier_p;

		struct lambda_grammar : public spirit::grammar<lambda_grammar>
		{
			template<typename T> struct definition
			{
				definition(const lambda_grammar&);

				const spirit::rule<T>& start()
				{
					return result;
				}

			private:
				const spirit::rule<T> result;
			};
		} lambda_p;

		struct term_grammar : public spirit::grammar<term_grammar>
		{
			template<typename T> struct definition
			{    
				definition(const term_grammar&);

				const spirit::rule<T>& start()
				{
					return result;
				}

			private:
				const spirit::rule<T> result;
			};
		} term_p;

		struct top_level_grammar : public spirit::grammar<top_level_grammar>
		{
			template<typename T> struct definition
			{
				definition(const top_level_grammar&);

				const spirit::rule<T>& start()
				{
					return result;
				}

			private:
				const spirit::rule<T> result;
			};
		} top_level_p;

		struct expression_grammar : public spirit::grammar<expression_grammar>
		{
			template<typename T> struct definition
			{
				definition(const expression_grammar&)
					: result(+term_p)
				{
				}

				const spirit::rule<T>& start()
				{
					return result;
				}

			private:
				const spirit::rule<T> result;
			};
		} expression_p;

		struct assignment_grammar : public spirit::grammar<assignment_grammar>
		{
			template<typename T> struct definition
			{
				definition(const assignment_grammar&)
					: result(identifier_p >> '=' >> expression_p >> ';')
				{
				}

				const spirit::rule<T>& start()
				{
					return result;
				}

			private: 
				const spirit::rule<T> result;
			};
		} assignment_p;

		class lambda_c
		{
		public:
			lambda_c(optional<const lax>& h, const name_space& _context)
				: context(child_name_space(_context)),
				_handle(h)
			{
			}

			template <typename I> void operator()(I begin, I end) const;

			mutable child_name_space context;

		private:
			mutable optional<const lax>& _handle;
		};

		class top_level_c
		{
		public:
			explicit top_level_c(optional<const lax>& h, name_space& _context)
				: context(_context),
				_handle(h)
			{
			}

			template <typename I> void operator()(I begin, I end) const;

			mutable name_space& context;

		private:
			mutable optional<const lax>& _handle;
		};

		class real_c
		{
		public:
			void operator()(const double& _) const
			{
				_handle = optional<const lax>(builtins::Kn()(_context.order())(_));
			}

			real_c(const name_space& context, optional<const lax>& h)
				: _context(context),
				_handle(h)
			{
			}

		private:
			const name_space& _context;
			optional<const lax>& _handle;
		};

		class character_c
		{
		public:
			template<typename I> void operator()(I begin, I) const
			{    
				_handle = optional<const lax>(builtins::Kn()(_context.order())(char(*begin)));
			}

			character_c(const name_space& context, optional<const lax>& h)
				: _context(context),
				_handle(h)
			{
			}

		private:
			const name_space& _context;
			optional<const lax>& _handle;
		};

		class string_c
		{
		public:
			string_c(const name_space& context, optional<const lax>& h)
				: _context(context),
				_handle(h)
			{
			}

			template<typename I> void operator()(I begin, I end) const
			{    
				_handle = optional<const lax>(builtins::Kn()(_context.order())(string(begin, end)));
			}

		private:
			const name_space& _context;
			optional<const lax>& _handle;
		};

		class boolean_c
		{
		public:
			boolean_c(const name_space& context, optional<const lax>& h)
				: _context(context),
				_handle(h)
			{
			}

			template<typename I> void operator()(I begin, I end) const
			{    
				_handle = optional<const lax>(builtins::Kn()(_context.order())(string(begin, end) == "true"));
			}

		private:
			const name_space& _context;
			optional<const lax>& _handle;
		};

		class define_identifier_c
		{
		public:
			explicit define_identifier_c(string& s)
				: _string(s)
			{
			}

			template<typename I> void operator()(I begin, I end) const
			{
				_string = string(begin, end);
			}

		private:
			string& _string;
		};

		class reference_identifier_c
		{
		public:
			reference_identifier_c(const name_space& context, optional<const lax>& h) 
				: _context(context),
				_handle(h)
			{
			}

			template<typename I> void operator()(I begin, I end) const
			{
				const string s(begin, end);
				_handle = optional<const lax>(_context[s]);
			}

		private:
			const name_space& _context;
			mutable optional<const lax>& _handle;
		};

		class term_c
		{
		public:
			term_c(const name_space& context, optional<const lax>& h)
				: _context(context),
				_handle(h)
			{
			}

			template <typename I> void operator()(I begin, I end) const;


		private:
			const name_space& _context;
			optional<const lax>& _handle;
		};

		class expression_c
		{
		public:
			expression_c(const name_space& context, optional<const lax>& h)
				: _context(context),
				_handle(h)
			{
			}

			template<typename I> void operator()(I begin, I end) const
			{    
				spirit::parse(begin, end, +term_p[term_c(_context, _handle)], spirit::space_p);
			}

		private:
			const name_space& _context;
			optional<const lax>& _handle;
		};

		class pre_assignment_c
		{
		public:
			explicit pre_assignment_c(name_space& context, const bool top_level)
				: _context(context),
				_top_level(top_level)
			{
			}

			template<typename I> void operator()(I begin, I end) const
			{
				string identifier;
				spirit::parse(begin, end, identifier_p[define_identifier_c(identifier)] >> '=' >> expression_p >> ';', spirit::space_p);
				_context.insert_indirect(identifier);
			}

		private:
			name_space& _context;
			const bool _top_level;
		};

		class assignment_c
		{
		public:
			explicit assignment_c(const name_space& context)
				: _context(context)
			{
			}

			template <typename I> void operator()(I begin, I end) const
			{
				string identifier;
				optional<const lax> expression;
				spirit::parse(begin, end, identifier_p[define_identifier_c(identifier)] >> '=' >> expression_p[expression_c(_context, expression)] >> ';', spirit::space_p);
				shared_ptr<const values::definition> p(dynamic_pointer_cast<const values::definition>(_context[identifier].value()));
				p->assign(*expression);
			}

		private:
			const name_space& _context;
		};

		template <class T> lambda_grammar::definition<T>::definition(const lambda_grammar&)
			: result(*assignment_p >> expression_p)
		{
		}

		template <class T> top_level_grammar::definition<T>::definition(const top_level_grammar&)
			: result(*assignment_p >> !expression_p)
		{
		}

		template <typename I> void term_c::operator()(I begin, I end) const
		{    
			optional<const lax> argument;

			spirit::parse(begin, end,
				('{' >> lambda_p[lambda_c(argument, _context)] >> '}')
				| ('(' >> expression_p[expression_c(_context, argument)] >> ')')
				| spirit::lexeme_d[spirit::confix_p('\"', (*spirit::anychar_p)[string_c(_context, argument)], '\"')]
				| spirit::lexeme_d[spirit::confix_p('\'', spirit::anychar_p[character_c(_context, argument)], '\'')]
				| spirit::lexeme_d[*spirit::ch_p(':') >> spirit::ch_p('?')][reference_identifier_c(_context, argument)]
				| (spirit::str_p("true") | spirit::str_p("false"))[boolean_c(_context, argument)]
				| spirit::lexeme_d[*spirit::ch_p(':') >> identifier_p][reference_identifier_c(_context, argument)]
				| spirit::real_p[real_c(_context, argument)], spirit::space_p);

				_handle = _handle ? optional<const lax>(builtins::Sn()(_context.order())(*_handle)(*argument)) : argument;
		}

		template <class T> term_grammar::definition<T>::definition(const term_grammar&)
			: result(
			('{' >> lambda_p >> '}')
			| ('(' >> expression_p >> ')')
			| spirit::lexeme_d[spirit::confix_p('\"', *spirit::anychar_p, '\"')]
			| spirit::lexeme_d[spirit::confix_p('\'', spirit::anychar_p, '\'')]
			| spirit::lexeme_d[*spirit::ch_p(':') >> '?']
			| spirit::lexeme_d[*spirit::ch_p(':') >> identifier_p]
			| spirit::real_p)
			{
			}

			template <typename I> void lambda_c::operator()(I begin, I end) const
			{
				optional<const lax> lex;
				spirit::parse(begin, end, *assignment_p[pre_assignment_c(context, false)] >> expression_p, spirit::space_p);
				spirit::parse(begin, end, *assignment_p[assignment_c(context)] >> expression_p[expression_c(context, lex)], spirit::space_p);
				_handle = lex;
			}

			template <typename I> void top_level_c::operator()(I begin, I end) const
			{
				optional<const lax> lex;
				spirit::parse(begin, end, *assignment_p[pre_assignment_c(context, true)] >> !expression_p, spirit::space_p);
				spirit::parse(begin, end, *assignment_p[assignment_c(context)] >> !expression_p[expression_c(context, lex)], spirit::space_p);
				_handle = lex;
			}

			template <typename I> const optional<const lax> parse(I begin, I end, name_space& context)
			{
				engine::current_context() = &context;
				optional<const lax> result;
				if (!spirit::parse(begin, end, top_level_p[top_level_c(result, context)], spirit::space_p).full)
					throw "Parse failed";
				return result;
			}

			const optional<const lax> parse(const std::string& s, name_space& context)
			{
				return parse(s.begin(), s.end(), context);
			}

			void parse_and_evaluate(const std::string& s, name_space& context, const bool silent)
			{
				engine::reset();
				if (!silent)
					log_cout << setprecision(15);
				try
				{
					const optional<lax> result(parse(s, context));
					if (result)
					{
						if (!silent)
						{
							log_cout << "=> " << (*result) << endl;
							log_cout << "-> " << result->evaluated() << endl;
						}
						else
						{
							result->evaluated();
						}
					}
					if (!silent)
						log_cout << endl;
				}
				catch (const char* _)
				{
					log_cout <<	"Exception:	" << _ << endl << endl;
				}
				catch (const string&  _)
				{
					log_cout <<	"Exception:	" << _ << endl << endl;
				}
				catch (...)
				{
					log_cout << "Exception: (unknown type)" << endl;
				}
			}

			void include_file(const string& file_name, name_space& context)
			{
				string code;
				string buffer;
				ifstream file(file_name.c_str());
				while (getline(file, buffer))
					code += buffer + '\n';
				file.close();
				log_cout << endl << "Parsing " << file_name << " ..." << endl;
				parse_and_evaluate(code, context, true);
			}
	}
}
