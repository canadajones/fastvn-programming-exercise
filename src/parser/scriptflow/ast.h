#ifndef SCRIPTFLOW_AST_HEADER
#define SCRIPTFLOW_AST_HEADER

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>


namespace scriptflow {
    namespace ast {
    
    namespace x3 = boost::spirit::x3;

    struct Speaker : x3::position_tagged {
        std::string name;
        boost::optional<std::string> expression;
    };

    struct Statement {
        Speaker speaker;
        std::string speech;
    };

    struct Dialogue {
        std::vector<Statement> statements;
    };


}}

#endif