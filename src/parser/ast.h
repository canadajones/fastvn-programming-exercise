#ifndef PARSER_AST_HEADER
#define PARSER_AST_HEADER

#include <string>
#include <string_view>
#include <unordered_map>
#include <map>
#include <optional>
#include <concepts>
#include <ranges>
#include <variant>


#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

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
	
	struct Array : x3::position_tagged {
		std::vector<Value> values;
	};

	struct Object : x3::position_tagged {
		std::unordered_map<std::string, Value> values;
	};

	struct Identifier : x3::position_tagged {
		boost::optional<std::string> ns;
		std::string id;

		explicit Identifier(std::string ns, std::string id) : ns{ns}, id{id} {};
		explicit Identifier() = default;

		std::string prettyprint() const {
			return ns.value_or("") + "." + id;
		}

		bool operator==(const Identifier& rhs) const {
			return (ns == rhs.ns) && (id == rhs.id);
		}
	};
}}

template<>
struct std::hash<script::ast::Identifier> {
	auto operator()(const script::ast::Identifier& ident) const {
		return std::hash<std::string>{}(ident.prettyprint());
	}
};

namespace script { namespace ast {
	struct List : boost::spirit::x3::position_tagged {
		std::unordered_map<Identifier, Value> entries;
	};
	
	using ObjectKeyVal = std::pair<std::string, Value>;

	using ListKeyVal = std::pair<Identifier, Value>;
	



	struct LineDeclaration : x3::position_tagged {
		std::string type;
		std::string name;
		List list;
	};

	struct BlockDeclaration : x3::position_tagged {
		std::string type;
		std::string name;
		List list;
		std::string block;
	};

	struct ScriptVersion : x3::position_tagged {
		int major;
		int minor;
		int patch;
	};

	/*
	struct UsingDeclaration : x3::position_tagged {
		std::string name;
	};
	*/

	using UsingDeclaration = std::string;

	struct Script : x3::position_tagged {
		ScriptVersion version;
		std::vector<std::variant<LineDeclaration, BlockDeclaration, UsingDeclaration>> contents;
	};

}}

	
#endif