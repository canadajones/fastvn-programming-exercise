#include "parser/parser.h"
#include "parser_def.h"
#include "config.h"

namespace script { namespace parser
{
    //BOOST_SPIRIT_INSTANTIATE(list_type, iterator_type, context_type);
    BOOST_SPIRIT_INSTANTIATE(obj_type, iterator_type, context_type);
}}