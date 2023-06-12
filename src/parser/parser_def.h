#ifndef PARSER_LIST_DEFINITION_HEADER
#define PARSER_LIST_DEFINITION_HEADER
#include <iostream>
//#define BOOST_SPIRIT_X3_DEBUG

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>



#include "ast.h"
#include "ast_adapted.h"
#include "boost/spirit/home/x3/auxiliary/eps.hpp"
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
		using x3::skip;

		using ascii::char_;

	


		// List parser
		// Parser declarations
		struct script_value_tag;
		struct script_array_tag;
		struct script_object_key_value_tag;
		struct script_object_tag;
		struct identifier_tag;
		struct script_list_key_value_tag;
		struct script_list_inner_tag;
		struct ident_chars_tag;

		x3::rule<script_value_tag, ast::Value> value = "value";
		
		x3::rule<script_array_tag, ast::Array> array = "array";
		
		x3::rule<script_object_key_value_tag, ast::ObjectKeyVal> object_key_value = "object_key_value";

		x3::rule<script_object_tag, ast::Object> object = "object";

		x3::rule<script_list_key_value_tag, ast::ListKeyVal> list_key_value = "list_key_value";

		x3::rule<ident_chars_tag, std::string> ident_chars = "ident_chars";

		x3::rule<identifier_tag, ast::Identifier> identifier = "identifier";

		x3::rule<script_list_inner_tag, ast::List> inner_list = "list";

		list_type list = "list";


		

		// Parser definitions
		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];
		
		// standard programming identifier rules: letter first, then alphanumerics
		const auto ident_chars_def = lexeme[(x3::alpha | char_('_') | char_('$')) >> *(x3::alnum | char_('_') | char_('$'))];
		
		const auto value_def = x3::double_ | quoted_string | ident_chars | array | object;

		const auto array_def = '[' > (value % ',') > ']';

		const auto object_key_value_def = (quoted_string | ident_chars) > ':' > value;

		const auto object_def = '{' > (object_key_value % ',') > '}';

		const auto identifier_def = lexeme[-(*(ident_chars) >> '.') > *(ident_chars)];
		
		const auto list_key_value_def = identifier > '=' > value;

		const auto inner_list_def = '(' > (list_key_value % ',') > ')';

		const auto list_def = inner_list_def;

		BOOST_SPIRIT_DEFINE(value, array, object, object_key_value, identifier, list_key_value, ident_chars, inner_list, list);

		struct script_value_tag : x3::annotate_on_success {};
		struct script_array_tag : x3::annotate_on_success {};
		struct script_object_key_value_tag : x3::annotate_on_success {};
		struct script_object_tag : x3::annotate_on_success {};
		struct identifier_tag : x3::annotate_on_success {};
		struct script_list_key_value_tag : x3::annotate_on_success {};
		struct script_list_inner_tag : x3::annotate_on_success {};
		struct script_list_tag : x3::annotate_on_success {};

		// Parser for overall structure
		
		
		struct version_tag;
		struct using_declaration_tag;
		struct line_declaration_tag;
		struct block_contests_tag;
		struct block_declaration_tag;
		
		
		x3::rule<version_tag, ast::ScriptVersion> version = "version";
		
		x3::rule<using_declaration_tag, ast::UsingDeclaration> using_declaration = "using_declaration";
		
		x3::rule<line_declaration_tag, ast::LineDeclaration> line_declaration = "line_declaration";
		
		x3::rule<block_contests_tag, std::string> block_contents = "block_contents";
		
		x3::rule<block_declaration_tag, ast::BlockDeclaration> block_declaration = "block_declaration";
				
		script_type script = "script";



		const auto version_def = x3::lit("script") >> "version" >> x3::int_ >> "." >> x3::int_ >> "." >> x3::int_ >> ";";
		
		const auto using_declaration_def = x3::lit("using") > skip(x3::lit(' ') | '\t')[ident_chars > x3::eol];
		
		const auto line_declaration_def = identifier >> ident_chars >> list >> skip(x3::lit(' ') | '\t')[x3::eps >> x3::eol];
		
		const auto block_contents_def = lexeme[("#{" >> *(!lit("}#") >> char_) >> "}#")];

		const auto block_declaration_def = identifier >> ident_chars >> (list > block_contents);

		const auto script_element = using_declaration | line_declaration | block_declaration;

		const auto script_def = x3::eps > version > +(script_element);
		

		BOOST_SPIRIT_DEFINE(version, using_declaration, line_declaration, block_contents, block_declaration, script);
		
		struct version_tag : x3::annotate_on_success {};
		struct using_declaration_tag : x3::annotate_on_success {};
		struct line_declaration_tag : x3::annotate_on_success {};
		struct block_declaration_tag : x3::annotate_on_success {};
		struct script_tag : x3::annotate_on_success, error_handler_base {};
		

		
	}

	parser::script_type getscript() {
		return parser::script;
	}	

	

}

#endif
