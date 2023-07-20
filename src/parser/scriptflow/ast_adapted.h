#ifndef SCRIPTFLOW_AST_ADAPTED_HEADER
#define SCRIPTFLOW_AST_ADAPTED_HEADER


#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "ast.h"
#include "boost/fusion/adapted/struct/adapt_struct.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    scriptflow::ast::Speaker,
    name,
    expression
)

BOOST_FUSION_ADAPT_STRUCT(
    scriptflow::ast::Statement,
    speaker,
    speech
)

BOOST_FUSION_ADAPT_STRUCT(
    scriptflow::ast::Annotation,
    name,
    arguments
)


BOOST_FUSION_ADAPT_STRUCT(
    scriptflow::ast::Dialogue,
    statements
)



#endif