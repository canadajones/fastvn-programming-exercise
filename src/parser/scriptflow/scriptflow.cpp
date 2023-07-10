#include "scriptflow.h"
#include "scriptflow_def.h"
#include "config.h"

namespace scriptflow { namespace parser
{
	BOOST_SPIRIT_INSTANTIATE(dialogue_type, iterator_type, context_type)

}}