#ifndef PARSER_LIST_DEFINITION_HEADER
#define PARSER_LIST_DEFINITION_HEADER
#include <iostream>

#include "ast.h"
#include "ast_adapted.h"

#define BOOST_SPIRIT_X3_DEBUG
#include "parser.h"
#include <boost/spirit/home/x3.hpp>





namespace script
{
	namespace parser
	{   
		namespace x3 = boost::spirit::x3;
		namespace ascii = boost::spirit::x3::ascii;

		using x3::lit;
		using x3::lexeme;

		using ascii::char_;
		using ascii::string;

		struct script_value_class;
		struct script_array_class;
		struct script_object_key_value_class;
		struct script_list_key_value_class;
		

		x3::rule<script_value_class, ast::Value> value = "value";
		
		x3::rule<script_array_class, ast::Array> array = "array";
		
		x3::rule<script_object_key_value_class, ast::ObjectKeyVal> object_key_value = "object_key_value";

		//x3::rule<script_list_class, ast::Object> object = "object";

		x3::rule<script_list_key_value_class, std::string> list_key_value = "list_key_value";

		
		
		obj_type object = "object";
		list_type list = "list";

		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];

		const auto value_def = x3::double_ | quoted_string | array | object;

		const auto array_def = '[' >> (value % ',') >> ']';

		const auto object_key_value_def = quoted_string >> ':' >> value;

		const auto object_def = '{' >> (object_key_value % ',') >> '}';

		// standard programming identifier rules: letter first, then alphanumerics
		const auto ident_chars = x3::alpha >> *(x3::alnum);

		const auto identifier = lexeme[*(ident_chars) >> '.' >> *(ident_chars)];
		
		const auto list_key_value_def = quoted_string >> '=' >> value;

		const auto list_def = '(' >> (list_key_value % ',') >> ')';

		BOOST_SPIRIT_DEFINE(value, array, object, object_key_value, list_key_value, list);
	}

	parser::list_type getlist() {
		return parser::list;
	}
	
	parser::obj_type getobj() {
		return parser::object;
	}

}

#endif
