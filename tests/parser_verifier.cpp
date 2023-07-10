#include <iostream>
#include <ranges>


#include <boost/spirit/home/x3.hpp>
#include <stdexcept>

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


#include "versioning/versioning.h"
#include "parser/metasyntax/version.h"




namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;


struct print_visitor {
	
	using result_type = void;

	metasyntax::ast::list_printer lp{std::cout};
	
	void operator()(const metasyntax::ast::UsingDeclaration& use) const {
		std::cout << "Using: " << use.name << "\n";

	}
	void operator()(const metasyntax::ast::LineDeclaration& line) const {
		std::cout << "Line: " << line.type.stringify() << " " << line.name << "\n";
		std::cout << "\tList: ";
		lp(line.list);	
	}
	void operator()(const metasyntax::ast::BlockDeclaration& block) const {
		std::cout << "Line: " << block.type.stringify() << " " << block.name << "\n";
		std::cout << "\tList: ";
		lp(block.list);

		std::cout << "\n Block: " << block.block << std::endl;	
	}
};



int main() {
	
	std::string file = "sample.txt";
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
		std::cout << "incomplete parsing\n";
	}

	// verify that script version matches internal parser version
	std::string versionString = "semver::metasyntax::v" + document.version.prettyprint();
	feature_versioning::FeatureRequirement requirement{versionString, feature_versioning::default_comparisons::default_version_comparator};

	if (!requirement.isSatisfiedBy(metasyntaxVersion)) {
		throw std::runtime_error("Metasyntax version does not match script version");
	}


	// ensure that the parser actually matched everything in sample.txt

	assert(boost::get<metasyntax::ast::UsingDeclaration>document.contents[0] == )	
	

	
}