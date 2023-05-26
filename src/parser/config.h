#ifndef PARSER_CONFIG_HEADER
#define PARSER_CONFIG_HEADER

#include <boost/spirit/home/x3.hpp>

namespace script { namespace parser
{
    namespace x3 = boost::spirit::x3;

    using iterator_type = std::string::const_iterator;
    using context_type = x3::phrase_parse_context<x3::ascii::space_type>::type;
}}

#endif
