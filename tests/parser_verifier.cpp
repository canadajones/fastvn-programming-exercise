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

#include "util/files.h"

#include "parser/printer.h"




namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;




int main() {
	//std::string test = R"( (a.a = 2.0, b = {test1 : "works", "test2": 2.0, nested: { "hello" : 6.9}}) )";
	
	std::string test = loadFileToString("sample.txt");
	
	using script::parser::iterator_type;

	iterator_type start = test.cbegin();
	iterator_type end = test.cend();
	
	using script::parser::error_handler_type;
	using script::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, std::string("Hello!")); 

	const auto parser = x3::with<script::parser::error_handler_tag>(std::ref(error_handler))[script::getscript()];





	script::ast::Script script;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, script);


	if (result) {
		std::cout << "works!" << std::endl;

		for (auto ch : error_handler.position_of(script)) {
			std::cout << ch;
		}
	}
	

	

	

	
}