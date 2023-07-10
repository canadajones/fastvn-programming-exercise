#ifndef METASYNTAX_INTERFACE_HEADER
#define METASYNTAX_INTERFACE_HEADER

#include <boost/spirit/home/x3.hpp>

#include "ast.h"


namespace metasyntax {
	namespace x3 = boost::spirit::x3;

	namespace parser
	{

		struct document_tag;
		
		using document_type = x3::rule<document_tag, ast::Document>;
		
		BOOST_SPIRIT_DECLARE(document_type);
	
	}

	parser::document_type getDocumentParser();

}


#endif