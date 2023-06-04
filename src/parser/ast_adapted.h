#ifndef PARSER_AST_ADAPTED_HEADER
#define PARSER_AST_ADAPTED_HEADER

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "ast.h"
#include "boost/fusion/adapted/struct/adapt_struct.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::Object,
    values
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::Array,
    values
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::Identifier,
    ns,
    id
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::List,
    entries
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::LineDeclaration,
    type,
    name,
    list
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::BlockDeclaration,
    type,
    name,
    list,
    block
)

BOOST_FUSION_ADAPT_STRUCT(
    script::ast::ScriptVersion,
    major,
    minor,
    patch
)
/*
BOOST_FUSION_ADAPT_STRUCT(
    script::ast::UsingDeclaration,
    name
)
*/
BOOST_FUSION_ADAPT_STRUCT(
    script::ast::Script,
    version,
    contents
)

#endif