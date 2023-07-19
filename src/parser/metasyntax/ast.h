#ifndef METASYNTAX_AST_HEADER
#define METASYNTAX_AST_HEADER

#include "boost/none_t.hpp"
#include "boost/variant/detail/apply_visitor_unary.hpp"
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

namespace metasyntax { 
	namespace ast {
		namespace x3 = boost::spirit::x3;
		
		struct NamespacedIdentifier : x3::position_tagged {
			boost::optional<std::string> ns;
			std::string id;

			explicit NamespacedIdentifier(const std::string& ns, const std::string& id) : ns{ns}, id{id} {};
			explicit NamespacedIdentifier(const boost::none_t& ns, const std::string& id) : ns{ns}, id{id} {};
			explicit NamespacedIdentifier() = default;

			std::string stringify() const {
				return ns.value_or("") + "." + id;
			}

			bool operator==(const NamespacedIdentifier& rhs) const {
				return (ns == rhs.ns) && (id == rhs.id);
			}
		};
}}

template<>
struct std::hash<metasyntax::ast::NamespacedIdentifier> {
	auto operator()(const metasyntax::ast::NamespacedIdentifier& ident) const {
		return std::hash<std::string>{}(ident.stringify());
	}
};

namespace metasyntax {
	namespace ast {

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
		using ObjectKeyVal = std::pair<std::string, Value>;
		
		
		struct List : boost::spirit::x3::position_tagged {
			std::unordered_map<NamespacedIdentifier, Value> entries;
		};
		using ListKeyVal = std::pair<NamespacedIdentifier, Value>;
		


		struct DocumentVersion : x3::position_tagged {
			int major;
			int minor;
			int patch;

			std::string prettyprint() const {
				return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
			}

			bool operator==(const DocumentVersion& rhs) const {
				return major == rhs.major && minor == rhs.major && patch == rhs.patch;
			} 
		};

		struct UsingDeclaration : x3::position_tagged {
			std::string name;

			explicit UsingDeclaration(const std::string& name) : name{name} {};
			explicit UsingDeclaration() = default;

			bool operator==(const UsingDeclaration& rhs) const {
				return name == rhs.name;
			} 
		};

		struct LineDeclaration : x3::position_tagged {
			NamespacedIdentifier type;
			std::string name;
			List list;

			LineDeclaration(const NamespacedIdentifier& type, const std::string& name, const List& list) : type{type}, name{name}, list{list} {};
			LineDeclaration() = default;

			bool operator==(const LineDeclaration& rhs) const {
				return type == rhs.type && name == rhs.name;
			} 
		};

		struct BlockDeclaration : x3::position_tagged {
			NamespacedIdentifier type;
			std::string name;
			List list;
			std::string block;

			BlockDeclaration(const NamespacedIdentifier& type, const std::string& name, const List& list, const std::string& block) : type{type}, name{name}, list{list}, block{block} {};
			BlockDeclaration() = default;
			bool operator==(const BlockDeclaration& rhs) const {
				return type == rhs.type && name == rhs.name && block == rhs.block;
			} 
		};

		struct Document : x3::position_tagged {
			DocumentVersion version;
			std::vector<x3::variant<LineDeclaration, BlockDeclaration, UsingDeclaration>> contents;
		};
}}

	
#endif