#ifndef METASYNTAX_CONFIG_HEADER
#define METASYNTAX_CONFIG_HEADER

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#include "error_handler.h"


namespace scriptflow { 
    namespace parser {
    namespace x3 = boost::spirit::x3;

    using iterator_type = std::string::const_iterator;
    using phrase_context_type = x3::phrase_parse_context<x3::ascii::space_type>::type;
    
   
 

    // Our Error Handler
    using error_handler_type = error_handler<iterator_type>;

    using position_cache = boost::spirit::x3::position_cache<std::vector<iterator_type>>;

    using context_type =  x3::context<error_handler_tag, std::reference_wrapper<error_handler_type>, phrase_context_type>;

    

    

}}

#endif
