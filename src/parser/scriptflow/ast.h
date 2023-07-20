#ifndef SCRIPTFLOW_AST_HEADER
#define SCRIPTFLOW_AST_HEADER

#include <string>
#include <vector>
#include <numeric>

#include <boost/optional.hpp>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>


namespace scriptflow {
	namespace ast {
	
	namespace x3 = boost::spirit::x3;

	struct Speaker : x3::position_tagged {
		std::string name;
		boost::optional<std::string> expression;

		explicit Speaker(const std::string& name) : name{name} {};
		explicit Speaker(const std::string& name, const boost::none_t&) : name{name} {};
		explicit Speaker(const std::string& name, const std::string& expression) : name{name}, expression{expression} {}
		explicit Speaker() = default;


		bool operator==(const Speaker& rhs) const {
			return name == rhs.name && expression == rhs.expression;
		}

		std::string debugPrint() {
			return "$Name: " + name + ";Expression:" + expression.value_or(std::string("")) + "$";
		}
	};

	struct Statement : x3::position_tagged {
		Speaker speaker;
		std::string speech;

		Statement(const Speaker& speaker, const std::string& speech) : speaker{speaker}, speech{speech} {}
		Statement() = default;

		bool operator==(const Statement& rhs) const {
			return speaker == rhs.speaker && speech == rhs.speech;
		}

		std::string debugPrint() {
			return "$Speaker:" + speaker.debugPrint() + ";Speech:" + speech + "$";
		}
		
	};

	struct Annotation : x3::position_tagged {
		std::string name;
		std::vector<std::string> arguments;

		Annotation(const std::string& name, const std::vector<std::string>& arguments) : name{name}, arguments{arguments} {}
		Annotation() = default;

		bool operator==(const Annotation& rhs) const {
			return name == rhs.name && arguments == rhs.arguments;
		}

		std::string debugPrint() {
			return "$Name: " + name + ";Arguments:" + std::accumulate(arguments.begin(), arguments.end(), std::string(""), [](const std::string& lhs, const std::string& rhs){return lhs + ";" + rhs;})+ "$";
		}
	};

	struct Dialogue : x3::position_tagged {
		std::vector<x3::variant<Statement, Annotation>> statements;
	};


}}

#endif