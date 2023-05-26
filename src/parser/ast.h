#ifndef PARSER_AST_HEADER
#define PARSER_AST_HEADER

#include <string>
#include <unordered_map>
#include <map>
#include <optional>
#include <concepts>
#include <ranges>


#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace script { namespace ast {

	namespace x3 = boost::spirit::x3;

	struct Array;
	struct Object;

	struct Value : x3::variant<
			double,
			std::string,
			x3::forward_ast<Array>,
			x3::forward_ast<Object>
		>
	{
		using base_type::base_type;
		using base_type::operator=;
	};
	
	struct Array {
		std::vector<Value> values;
	};

	struct Object {
		std::unordered_map<std::string, Value> values;
	};

	struct List {
		std::unordered_map<std::string, Value> entries;
	};
	using ObjectKeyVal = std::pair<std::string, Value>;

	using ListKeyVal = std::pair<std::string, Value>;
	
	

	using boost::fusion::operator<<;


}}

	
#endif