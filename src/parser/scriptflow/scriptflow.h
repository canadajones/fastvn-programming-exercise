#ifndef SCRIPTFLOW_INTERFACE_HEADER
#define SCRIPTFLOW_INTERFACE_HEADER


#include <boost/spirit/home/x3.hpp>

#include "ast.h"


namespace scriptflow {
	namespace x3 = boost::spirit::x3;

	namespace parser
	{
		struct dialogue_tag;
		
		using dialogue_type = x3::rule<dialogue_tag, ast::Dialogue>;
		
		BOOST_SPIRIT_DECLARE(dialogue_type);

	
	}

	parser::dialogue_type getdialogue();

}

#endif