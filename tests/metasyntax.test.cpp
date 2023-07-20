#include <concepts>
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


namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;


struct print_visitor {
	
	using result_type = void;

	metasyntax::ast::line_printer lp{std::cout};
	
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

template<typename T>
void must_equal(decltype(metasyntax::ast::Document::contents)::value_type value, T target) {
	if (boost::get<T>(value) != target) {
		throw std::runtime_error(boost::core::demangle(typeid(T).name()) + ": Value does not equal target!");
	}
}

template<typename T>
concept HasName = requires(T t) { {t.name} -> std::convertible_to<std::string>; };

template<HasName T>
void must_equal(decltype(metasyntax::ast::Document::contents)::value_type value, T target) {
	auto v = boost::get<T>(value);
	if (v != target) {
		
		throw std::runtime_error(boost::core::demangle(typeid(T).name()) + ": " + v.name + " does not equal " + target.name + "!");
	}
}


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
	namespace ms = metasyntax::ast;
	must_equal(contents[0], ms::UsingDeclaration{"vnpge"});
	must_equal(contents[1], ms::LineDeclaration{ms::NamespacedIdentifier{boost::none, "location"}, "Park", ms::List()});
	must_equal(contents[2], ms::LineDeclaration{ms::NamespacedIdentifier{boost::none, "character"}, "Sara", ms::List()});
	must_equal(contents[3], ms::LineDeclaration{ms::NamespacedIdentifier{boost::none, "character"}, "Mark", ms::List()});
	
	const std::string test_string = "\n"
												 "\n"
    											 "\tSara, frustrated: \"Man, I wish we got more time off. When was the last time we went to the park like this?\"\n"
												 "\tMark, contemplative: \"Yeah, this blows. I hate having to run off just to get some fresh air.\"\n"
												 "\tSara : \"There has to be something we can do.\"\n"
												 "\t@: \"It's not fair for us to take the fall for what *they* did.\"\n"
												 "\t@, determined: \"That's it. I'm not keeping quiet any longer.\"\n"
												 "\t\n"
												 "\n"
												 "\tscene_change(Park_Conversation_05)\n"
												 "";

	must_equal(contents[4], ms::BlockDeclaration{ms::NamespacedIdentifier{boost::none, "scene"}, "Park_Conversation_04", ms::List(), test_string});

	std::cout << "Test successful." << std::endl;
}