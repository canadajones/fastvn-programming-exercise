#ifndef SCRIPTFLOW_ERROR_HANDLER_HEADER
#define SCRIPTFLOW_ERROR_HANDLER_HEADER

#include <vector>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

#include <unordered_map>

#include "scriptflow.h"

namespace scriptflow { 
	namespace parser {
		namespace x3 = boost::spirit::x3;

		
		// X3 Error Handler Utility
		template <typename Iterator>
		using error_handler = x3::error_handler<Iterator>;
		using error_handler_tag = x3::error_handler_tag;

		struct error_handler_base
		{
			error_handler_base();

			template <typename Iterator, typename Exception, typename Context>
			x3::error_handler_result on_error(
				Iterator& first, Iterator const& last
			, Exception const& x, Context const& context);

			std::unordered_map<std::string, std::string> id_map;
		};

		////////////////////////////////////////////////////////////////////////////
		// Implementation
		////////////////////////////////////////////////////////////////////////////

		inline error_handler_base::error_handler_base()
		{
			id_map["identifier"] = "Identifier";
			id_map["speaker"] = "Speaker";
			id_map["statement"] = "Statement";
			id_map["dialogue"] = "Dialogue";
			
		}

		template <typename Iterator, typename Exception, typename Context>
		inline x3::error_handler_result
		error_handler_base::on_error(
			Iterator& first, Iterator const& last
		, Exception const& x, Context const& context)
		{
			std::string which = x.which();
			auto iter = id_map.find(which);
			if (iter != id_map.end())
				which = iter->second;

			std::string message = "Error! Expecting: " + which + " here:";
			auto& error_handler = x3::get<error_handler_tag>(context).get();
			error_handler(x.where(), message);
			return x3::error_handler_result::fail;
		}
	}
}

#endif
