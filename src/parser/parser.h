#ifndef PARSER_INTERFACE_HEADER
#define PARSER_INTERFACE_HEADER

#include <boost/spirit/home/x3.hpp>

#include "ast.h"


namespace script {
	namespace x3 = boost::spirit::x3;

	namespace parser
	{
		struct script_list_tag;
		
		using list_type = x3::rule<script_list_tag, ast::List>;
		
		BOOST_SPIRIT_DECLARE(list_type);


		struct script_tag;
		
		using script_type = x3::rule<script_tag, ast::Script>;
		
		BOOST_SPIRIT_DECLARE(script_type);
	
	}
	parser::list_type getlist();  
	parser::script_type getscript();

}


#endif