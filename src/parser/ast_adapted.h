#ifndef PARSER_AST_ADAPTED_HEADER
#define PARSER_AST_ADAPTED_HEADER

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "ast.h"

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::Object,
    values
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::Array,
    values
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::List,
    entries
)

#endif