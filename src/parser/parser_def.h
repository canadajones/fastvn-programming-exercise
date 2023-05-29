#ifndef PARSER_LIST_DEFINITION_HEADER
#define PARSER_LIST_DEFINITION_HEADER
#include <iostream>

#include "ast.h"
#include "ast_adapted.h"

#define BOOST_SPIRIT_X3_DEBUG
#include "parser.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>




namespace script
{
	namespace parser
	{   
		namespace x3 = boost::spirit::x3;
		namespace ascii = boost::spirit::x3::ascii;

		using x3::lit;
		using x3::lexeme;

		using ascii::char_;

		// tag used to get the position cache from the context
		struct position_cache_tag;

		struct annotate_position
		{
			template <typename T, typename Iterator, typename Context>
			inline void on_success(Iterator const& first, Iterator const& last
			, T& ast, Context const& context)
			{
				auto& position_cache = x3::get<position_cache_tag>(context).get();
				position_cache.annotate(ast, first, last);
			}
		};


		// List parser
		// Parser declarations
		struct script_value_class;
		struct script_array_class;
		struct script_object_key_value_class;
		struct script_object_class;
		struct identifier_class;
		struct script_list_key_value_class;
		
		x3::rule<script_value_class, ast::Value> value = "value";
		
		x3::rule<script_array_class, ast::Array> array = "array";
		
		x3::rule<script_object_key_value_class, ast::ObjectKeyVal> object_key_value = "object_key_value";

		x3::rule<script_object_class, ast::Object> object = "object";

		x3::rule<script_list_key_value_class, ast::ListKeyVal> list_key_value = "list_key_value";

		x3::rule<identifier_class, ast::Identifier> identifier = "identifier";

		list_type list = "list";


		// Parser definitions
		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];

		const auto value_def = x3::double_ | quoted_string | array | object;

		const auto array_def = '[' >> (value % ',') >> ']';

		const auto object_key_value_def = quoted_string >> ':' >> value;

		const auto object_def = '{' >> (object_key_value % ',') >> '}';

		// standard programming identifier rules: letter first, then alphanumerics
		const auto ident_chars = x3::alpha >> *(x3::alnum);

		const auto identifier_def = lexeme[-(*(ident_chars) >> '.') >> *(ident_chars)];
		
		const auto list_key_value_def = identifier >> '=' >> value;

		const auto list_def = '(' >> (list_key_value % ',') >> ')';

		BOOST_SPIRIT_DEFINE(value, array, object, object_key_value, identifier, list_key_value, list);



		// Parser for overall structure


	}

	parser::list_type getlist() {
		return parser::list;
	}
	

}

#endif
