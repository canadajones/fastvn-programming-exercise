#ifndef PARSER_INTERFACE_HEADER
#define PARSER_INTERFACE_HEADER

#include <boost/spirit/home/x3.hpp>

#include "ast.h"


namespace script {
    namespace x3 = boost::spirit::x3;

    ///////////////////////////////////////////////////////////////////////////
    // rexpr public interface
    ///////////////////////////////////////////////////////////////////////////
    namespace parser
    {
        struct script_list_class;
        
        using list_type = x3::rule<script_list_class, ast::List>;
        
        BOOST_SPIRIT_DECLARE(list_type);

        struct script_obj_class;
        
        using obj_type = x3::rule<script_list_class, ast::Object>;
        
        BOOST_SPIRIT_DECLARE(obj_type);

    
    }
    parser::list_type getlist();
    parser::obj_type getobj();    
}


#endif