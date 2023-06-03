#ifndef PARSER_LIST_DEFINITION_HEADER
#define PARSER_LIST_DEFINITION_HEADER
#include <iostream>


#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "ast.h"
#include "ast_adapted.h"
#include "config.h"
#include "parser.h"
#include "parser/error_handler.h"
#include "parser/printer.h"




namespace script
{
	namespace parser
	{   
		namespace x3 = boost::spirit::x3;
		namespace ascii = boost::spirit::x3::ascii;

		using x3::lit;
		using x3::lexeme;

		using ascii::char_;

	


		// List parser
		// Parser declarations
		struct script_value_class;
		struct script_array_class;
		struct script_object_key_value_class;
		struct script_object_class;
		struct identifier_class;
		struct script_list_key_value_class;
		struct script_list_inner_class;
		
		x3::rule<script_value_class, ast::Value> value = "value";
		
		x3::rule<script_array_class, ast::Array> array = "array";
		
		x3::rule<script_object_key_value_class, ast::ObjectKeyVal> object_key_value = "object_key_value";

		x3::rule<script_object_class, ast::Object> object = "object";

		x3::rule<script_list_key_value_class, ast::ListKeyVal> list_key_value = "list_key_value";

		x3::rule<identifier_class, ast::Identifier> identifier = "identifier";

		x3::rule<script_list_inner_class, ast::List> inner_list = "list";

		list_type list = "list";


		// Parser definitions
		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];
		// standard programming identifier rules: letter first, then alphanumerics
		const auto ident_chars = x3::alpha >> *(x3::alnum);
		
		const auto value_def = x3::double_ | quoted_string | array | object;

		const auto array_def = '[' > (value % ',') > ']';

		const auto object_key_value_def = (quoted_string | ident_chars) > ':' > value;

		const auto object_def = '{' > (object_key_value % ',') > '}';

		const auto identifier_def = lexeme[-(*(ident_chars) >> '.') > *(ident_chars)];
		
		const auto list_key_value_def = identifier > '=' > value;

		const auto inner_list_def = '(' > (list_key_value % ',') > ')';

		const auto list_def = inner_list_def;

		BOOST_SPIRIT_DEFINE(value, array, object, object_key_value, identifier, list_key_value, inner_list,list);

		struct script_value_class : x3::annotate_on_success {};
		struct script_array_class : x3::annotate_on_success {};
		struct script_object_key_value_class : x3::annotate_on_success {};
		struct script_object_class : x3::annotate_on_success {};
		struct identifier_class : x3::annotate_on_success {};
		struct script_list_key_value_class : x3::annotate_on_success {};
		struct script_list_inner_class : x3::annotate_on_success {};
		struct script_list_class : x3::annotate_on_success, error_handler_base {};

		// Parser for overall structure

		
		struct line_declaration_class;
		struct block_declaration_class;

		x3::rule<line_declaration_class, ast::LineDeclaration> line_declaration = "line_declaration";
		x3::rule<block_declaration_class, ast::BlockDeclaration> block_declaration = "block_declaration";

		const auto line_declaration_def = (*ident_chars) >> (*ident_chars) >> list >> x3::eol;
		const auto block_declaration_def = (*ident_chars) >> (*ident_chars) >> list >> ("#{" >> (x3::char_ - "}#") >> "}#");
		


	}

	parser::list_type getlist() {
		return parser::list;
	}
	

}

#endif
