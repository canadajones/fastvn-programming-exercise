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
#include "util/output.h"

#include "parser/printer.h"




namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;


struct print_visitor {
	
	using result_type = void;

	script::ast::list_printer lp{std::cout};
	
	void operator()(const script::ast::UsingDeclaration& use) const {
		std::cout << "Using: " << use.name << "\n";

	}
	void operator()(const script::ast::LineDeclaration& line) const {
		std::cout << "Line: " << line.type.stringify() << " " << line.name << "\n";
		std::cout << "\tList: ";
		lp(line.list);	
	}
	void operator()(const script::ast::BlockDeclaration& block) const {
		std::cout << "Line: " << block.type.stringify() << " " << block.name << "\n";
		std::cout << "\tList: ";
		lp(block.list);

		std::cout << "\n Block: " << block.block << std::endl;	
	}
};



int main() {
	
	std::string file = "sample.txt";
	std::string test = loadFileToString(file);
	
	using script::parser::iterator_type;

	iterator_type start = test.cbegin();
	iterator_type end = test.cend();
	
	using script::parser::error_handler_type;
	using script::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, file); 

	const auto parser = x3::with<script::parser::error_handler_tag>(std::ref(error_handler))[script::getscript()];



	script::ast::Script script;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, script);


	if (result) {
		for (auto ch : error_handler.position_of(script)) {
			print(ch);
		}
		print("\n");
	}
	
	std::cout << script.version.prettyprint() << std::endl;
	
	print_visitor pv;
	for (auto elem : script.contents) {

		boost::apply_visitor(print_visitor{}, elem);
	}
	

	

	
}