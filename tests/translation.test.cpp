#include <string>

#include "boost/spirit/home/x3/char/char_class.hpp"
#include "boost/spirit/home/x3/core/parse.hpp"

#include "util/files.h"
#include "util/output.h"

#include "parser/metasyntax/metasyntax.h"
#include "parser/metasyntax/ast.h"
#include "parser/metasyntax/ast_adapted.h"
#include "parser/metasyntax/config.h"
#include "parser/metasyntax/error_handler.h"
#include "parser/metasyntax/printer.h"

#include "parser/translation.h"

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;



int main() {
    std::string file = "metasyntax.sample.txt";
	std::string test = loadFileToString(file);
	
	using metasyntax::parser::iterator_type;

	iterator_type start = test.cbegin();
	iterator_type end = test.cend();
	
	using metasyntax::parser::error_handler_type;
	using metasyntax::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, file); 

	const auto parser = x3::with<metasyntax::parser::error_handler_tag>(std::ref(error_handler))[metasyntax::getDocumentParser()];



	metasyntax::ast::Document document;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, document);

	if (!result) {
		std::cout << "Parsing incomplete, test halted." << std::endl;
		return -1;
	}

	
	 if (document.version.prettyprint() != "0.0.1") {
		throw std::runtime_error("VersionCheck: " + document.version.prettyprint() + " does not equal 0.0.1");
	 }
     


	// ensure that the parser actually matched everything in sample.txt
	// todo: currently, lists are not checked (recursive and miserable to check)
	auto& contents = document.contents;
    
    translation::begin_peel(document);

}