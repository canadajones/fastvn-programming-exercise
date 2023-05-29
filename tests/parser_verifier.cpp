#include <iostream>
#include <ranges>


#include <boost/spirit/home/x3.hpp>

#include "boost/spirit/home/x3/char/char_class.hpp"
#include "boost/spirit/home/x3/core/parse.hpp"


#include "parser/parser.h"
#include "parser/ast.h"
#include "parser/ast_adapted.h"

#include "parser/printer.h"


namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;


int main() {
    std::string test = R"( (a.a = 2.0, b = {"test1" : "works", "test2": 2.0, "nested": {"hello" : 6.9}}) )";

    auto b = test.cbegin();
    auto e = test.cend();
    
    script::ast::List list;

    auto result = x3::phrase_parse(b, e, script::getlist(), ascii::space, list);

    if (b != e) {
        std::cout << std::boolalpha << result << "\n";
        
        for (auto c1 = b; c1 != e; c1++ ) {
            std::cout << *c1;
        }
        std::cout << std::endl;
    }

    script::ast::list_printer lp(std::cout, 0);

    lp(list);

    
}