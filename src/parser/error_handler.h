/*=============================================================================
	Copyright (c) 2001-2015 Joel de Guzman

	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef PARSER_ERROR_HANDLER_HEADER
#define PARSER_ERROR_HANDLER_HEADER

#include "parser.h"

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

#include <unordered_map>

namespace script { namespace parser
{
	namespace x3 = boost::spirit::x3;

	////////////////////////////////////////////////////////////////////////////
	//  Our error handler
	////////////////////////////////////////////////////////////////////////////
	// X3 Error Handler Utility
	template <typename Iterator>
	using error_handler = x3::error_handler<Iterator>;

	// tag used to get our error handler from the context
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
		id_map["list"] = "Expression list";
		id_map["value"] = "Value";
		id_map["list_key_value"] = "List key value pair";
		id_map["array"] = "Array";
		id_map["object_key_value"] = "Object key value pair";
		id_map["object"] = "Object";
		id_map["identifier"] = "Identifier";
		
		
		id_map["version"] = "Version identifier";
		id_map["using_declaration"] = "Using declaration";
		id_map["line_declaration"] = "Line declaration";
		id_map["block_declaration"] = "Block declaration";
		id_map["script"] = "Script";
		
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
}}

#endif
