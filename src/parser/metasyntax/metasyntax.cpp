#include "metasyntax.h"
#include "metasyntax_def.h"
#include "config.h"

namespace metasyntax { namespace parser
{
	BOOST_SPIRIT_INSTANTIATE(document_type, iterator_type, context_type)

}}