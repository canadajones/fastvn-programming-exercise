#ifndef METASYNTAX_AST_ADAPTED_HEADER
#define METASYNTAX_AST_ADAPTED_HEADER

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "ast.h"
#include "boost/fusion/adapted/struct/adapt_struct.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::Object,
    values
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::Array,
    values
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::NamespacedIdentifier,
    ns,
    id
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::List,
    entries
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::DocumentVersion,
    major,
    minor,
    patch
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::UsingDeclaration,
    name
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::LineDeclaration,
    type,
    name,
    list
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::BlockDeclaration,
    type,
    name,
    list,
    block
)

BOOST_FUSION_ADAPT_STRUCT(
    metasyntax::ast::Document,
    version,
    contents
)

#endif