#ifndef METASYNTAX_PARSER_DEFINITION_HEADER
#define METASYNTAX_PARSER_DEFINITION_HEADER
#include <iostream>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include "boost/spirit/home/x3/auxiliary/eps.hpp"


#include "ast.h"
#include "ast_adapted.h"
#include "config.h"
#include "error_handler.h"

#include "metasyntax.h"


namespace metasyntax {
	namespace parser {

		namespace x3 = boost::spirit::x3;
		namespace ascii = boost::spirit::x3::ascii;

		using x3::lit;
		using x3::lexeme;
		using x3::skip;

		using ascii::char_;

	


		// List parser
		// Parser declarations
		struct value_tag;
		struct array_tag;
		struct object_key_value_tag;
		struct object_tag;
		struct identifier_tag;
		struct list_key_value_tag;
		struct list_inner_tag;
		struct identifier_tag;
		struct list_tag;

		x3::rule<value_tag, ast::Value> value = "value";
		
		x3::rule<array_tag, ast::Array> array = "array";
		
		x3::rule<object_key_value_tag, ast::ObjectKeyVal> object_key_value = "object_key_value";

		x3::rule<object_tag, ast::Object> object = "object";

		x3::rule<list_key_value_tag, ast::ListKeyVal> list_key_value = "list_key_value";

		x3::rule<identifier_tag, std::string> identifier = "identifier";

		x3::rule<identifier_tag, ast::NamespacedIdentifier> namespaced_identifier = "identifier";

		x3::rule<list_inner_tag, ast::List> inner_list = "list";

		
	
		

		x3::rule<list_tag, ast::List> list = "list";


		

		// Parser definitions
		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];
		
		// standard programming identifier rules: letter first, then alphanumerics
		const auto identifier_def = lexeme[(x3::alpha | char_('_') | char_('$')) >> *(x3::alnum | char_('_') | char_('$'))];
		
		const auto value_def = x3::double_ | quoted_string | identifier | array | object;

		const auto array_def = '[' > (value % ',') > ']';

		const auto object_key_value_def = (quoted_string | identifier) > ':' > value;

		const auto object_def = '{' > (object_key_value % ',') > '}';

		const auto namespaced_identifier_def = lexeme[-(*(identifier) >> '.') > *(identifier)];
		
		const auto list_key_value_def = namespaced_identifier > '=' > value;

		const auto inner_list_def = '(' > (list_key_value % ',') > ')';

		const auto list_def = inner_list_def;

		BOOST_SPIRIT_DEFINE(value, array, object, object_key_value, namespaced_identifier, list_key_value, identifier, inner_list, list);

		struct value_tag : x3::annotate_on_success {};
		struct array_tag : x3::annotate_on_success {};
		struct object_key_value_tag : x3::annotate_on_success {};
		struct object_tag : x3::annotate_on_success {};
		struct identifier_tag : x3::annotate_on_success {};
		struct list_key_value_tag : x3::annotate_on_success {};
		struct list_inner_tag : x3::annotate_on_success {};
		struct list_tag : x3::annotate_on_success {};

		// Parser for overall structure
		
		
		struct version_tag;
		struct using_declaration_tag;
		struct line_declaration_tag;
		struct block_contests_tag;
		struct block_declaration_tag;
		
		
		x3::rule<version_tag, ast::DocumentVersion> version = "version";
		
		x3::rule<using_declaration_tag, ast::UsingDeclaration> using_declaration = "using_declaration";
		
		x3::rule<line_declaration_tag, ast::LineDeclaration> line_declaration = "line_declaration";
		
		x3::rule<block_contests_tag, std::string> block_contents = "block_contents";
		
		x3::rule<block_declaration_tag, ast::BlockDeclaration> block_declaration = "block_declaration";
				
		document_type document = "document";



		const auto version_def = x3::lit("script") >> "version" >> x3::int_ >> "." >> x3::int_ >> "." >> x3::int_ >> ";";
		
		const auto using_declaration_def = x3::lit("using") > skip(x3::lit(' ') | '\t')[identifier > x3::eol];
		
		const auto line_declaration_def = namespaced_identifier >> identifier >> list >> skip(x3::lit(' ') | '\t')[x3::eps >> x3::eol];
		
		const auto block_contents_def = lexeme[("#{" >> *(!lit("}#") >> char_) >> "}#")];

		const auto block_declaration_def = namespaced_identifier >> identifier >> (list > block_contents);

		const auto document_element = using_declaration | line_declaration | block_declaration;

		const auto document_def = x3::eps > version > +(document_element);
		

		BOOST_SPIRIT_DEFINE(version, using_declaration, line_declaration, block_contents, block_declaration, document);
		
		struct version_tag : x3::annotate_on_success {};
		struct using_declaration_tag : x3::annotate_on_success {};
		struct line_declaration_tag : x3::annotate_on_success {};
		struct block_declaration_tag : x3::annotate_on_success {};
		struct document_tag : x3::annotate_on_success, error_handler_base {};
		

		
	}

	parser::document_type getDocumentParser() {
		return parser::document;
	}	

	

}

#endif
