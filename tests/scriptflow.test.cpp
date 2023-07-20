#include <concepts>
#include <iostream>
#include <ranges>


#include <boost/spirit/home/x3.hpp>
#include <stdexcept>

#include "boost/spirit/home/x3/char/char_class.hpp"
#include "boost/spirit/home/x3/core/parse.hpp"

#include "util/files.h"
#include "util/output.h"

#include "parser/scriptflow/scriptflow.h"
#include "parser/scriptflow/ast.h"
#include "parser/scriptflow/ast_adapted.h"
#include "parser/scriptflow/config.h"
#include "parser/scriptflow/error_handler.h"




namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;




template<typename T>
void must_equal(decltype(scriptflow::ast::Dialogue::statements)::value_type value, T target) {
	auto v = boost::get<T>(value);
	if (v != target) {
		throw std::runtime_error(boost::core::demangle(typeid(T).name()) + ": Value does not equal target!\n" + v.debugPrint() + "\n" + target.debugPrint());
	}
}


int main() {
	
	std::string file = "scriptflow.sample.txt";
	std::string test = loadFileToString(file);
	
	using scriptflow::parser::iterator_type;

	iterator_type start = test.cbegin();
	iterator_type end = test.cend();
	
	using scriptflow::parser::error_handler_type;
	using scriptflow::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, file); 

	const auto parser = x3::with<scriptflow::parser::error_handler_tag>(std::ref(error_handler))[scriptflow::getDialogueParser()];



	scriptflow::ast::Dialogue dialogue;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, dialogue);

	if (!result) {
		std::cout << "Parsing incomplete, test halted." << std::endl;
		return -1;
	}

	
	
	auto& contents = dialogue.statements;
	namespace sc = scriptflow::ast;

	must_equal(contents[0], sc::Statement{sc::Speaker{"Sara", "frustrated"}, "Man, I wish we got more time off. When was the last time we went to the park like this?"});
	must_equal(contents[1], sc::Statement{sc::Speaker{"Mark", "contemplative"}, "Yeah, this blows. I hate having to run off just to get some fresh air."});
	must_equal(contents[2], sc::Statement{sc::Speaker{"Sara"}, "There has to be something we can do."});
	must_equal(contents[3], sc::Statement{sc::Speaker{"@"}, "It's not fair for us to take the fall for what *they* did."});
	must_equal(contents[4], sc::Statement{sc::Speaker{"@", "determined"}, "That's it. I'm not keeping quiet any longer."});
	must_equal(contents[5], sc::Annotation{"scene_change", {"Park_Conversation_05"}});


	std::cout << "Test successful." << std::endl;
}