#include <iostream>
#include <ranges>


#include <boost/spirit/home/x3.hpp>
#include <stdexcept>

#include "boost/spirit/home/x3/char/char_class.hpp"
#include "boost/spirit/home/x3/core/parse.hpp"


#include "parser/parser.h"
#include "parser/ast.h"
#include "parser/ast_adapted.h"
#include "parser/config.h"
#include "parser/error_handler.h"

#include "parser/printer.h"


namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;




int main() {
	std::string test = R"( (a.a = 2.0, b = {test1 : "works", "test2": 2.0, nested: { "hello" : 6.9}}) )";
	
	
	using script::parser::iterator_type;

	iterator_type start = test.cbegin();
	iterator_type end = test.cend();
	
	using script::parser::error_handler_type;
	using script::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, std::string("Hello!")); 

	const auto parser = x3::with<script::parser::error_handler_tag>(std::ref(error_handler))[script::getlist()];





	script::ast::List list;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, list);


	

	if (result) {
		script::ast::list_printer lp(std::cout, 0);
		lp(list);
	}

	

	
}