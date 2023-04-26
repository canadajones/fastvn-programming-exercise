#include <iostream>
#include <ranges>
#include <numeric>
#include <fstream>
#include <typeinfo>


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/bind/bind.hpp>
#include <boost/ref.hpp>
#include <variant>

#include "util/files.h"



namespace qi = boost::spirit::qi;
namespace lex = boost::spirit::lex;



enum struct ScriptTokenID : int {
	Whitespace = 1,
	Newline,
	Identifier,
	NamespacedIdentifier,
	BlockContents,
	VerIdent,
	ListBegin,
	ListEnd,
	Comma,
	Equals,
	ArrayBegin,
	ArrayEnd,
	ObjectBegin,
	ObjectEnd,
	ObjectSeparator,
	QuotedString,
	Number
};


constexpr auto enumString(ScriptTokenID tok) {
	switch(tok) {
	break;
		case ScriptTokenID::Whitespace:
			return "Whitespace";
		case ScriptTokenID::Newline:
			return "Newline";
		case ScriptTokenID::Identifier:
			return "Identifier";
		case ScriptTokenID::NamespacedIdentifier:
			return "NamespacedIdentifier";
		case ScriptTokenID::BlockContents:
			return "BlockContents";
		case ScriptTokenID::VerIdent:
			return "VerIdent";
		case ScriptTokenID::ListBegin:
			return "ListBegin";
		case ScriptTokenID::ListEnd:
			return "ListEnd";
		case ScriptTokenID::Comma:
			return "Comma";
		case ScriptTokenID::Equals:
			return "Equals";
		case ScriptTokenID::ArrayBegin:
			return "ArrayBegin";
		case ScriptTokenID::ArrayEnd:
			return "ArrayEnd";
		case ScriptTokenID::ObjectBegin:
			return "ObjectBegin";
		case ScriptTokenID::ObjectEnd:
			return "ObjectEnd";
		case ScriptTokenID::ObjectSeparator:
			return "ObjectSeparator";
		case ScriptTokenID::QuotedString:
			return "QuotedString";
		case ScriptTokenID::Number:
			return "Number";
		break;
		};
		return "avoidUB";
}

constexpr auto tokenID(ScriptTokenID id) {
	return static_cast<size_t>(id);
}


template <typename Lexer>
struct script_tokens : lex::lexer<Lexer>
{
	script_tokens()
	{
		// define patterns (lexer macros) to be used during token definition 
		// below
		this->self.add_pattern
			("IDENTIFIER", "[_a-zA-Z][_a-zA-Z0-9]*")
			("QUOTED_STRING", "\\\"([^\\\"\\\\]|\\.)*\\\"")
		;

		// define tokens and associate them with the lexer
		whitespace = "[ \t]";
		newline = "\n";
		identifier = "{IDENTIFIER}";
		namespacedIdentifier = "{IDENTIFIER}\\.{IDENTIFIER}";
		blockContents = "\\#\\{[^#]*\\}\\#";
		verIdent = "script version [0-9]*\\.[0-9]*\\.[0-9]*;";
		listBegin = "\\(";
		listEnd = "\\)";
		comma = ",";
		equals = "=";
		arrayBegin = "\\[";
		arrayEnd = "\\]";
		objectBegin = "\\{";
		objectEnd = "\\}";
		objectSeparator = "\\:";
		quotedString = "{QUOTED_STRING}";
		number = "[0-9]+\\.[0-9]+";

		this->self.add
			(whitespace, tokenID(ScriptTokenID::Whitespace))
			(newline, tokenID(ScriptTokenID::Newline))
			(namespacedIdentifier, tokenID(ScriptTokenID::NamespacedIdentifier))
			(identifier, tokenID(ScriptTokenID::Identifier))
			(blockContents, tokenID(ScriptTokenID::BlockContents))
			(verIdent, tokenID(ScriptTokenID::VerIdent))      
			(listBegin, tokenID(ScriptTokenID::ListBegin))
			(listEnd, tokenID(ScriptTokenID::ListEnd))
			(comma, tokenID(ScriptTokenID::Comma))
			(equals, tokenID(ScriptTokenID::Equals))
			(arrayBegin, tokenID(ScriptTokenID::ArrayBegin))
			(arrayEnd, tokenID(ScriptTokenID::ArrayEnd))
			(objectBegin, tokenID(ScriptTokenID::ObjectBegin))
			(objectEnd, tokenID(ScriptTokenID::ObjectEnd))
			(objectSeparator, tokenID(ScriptTokenID::ObjectSeparator))
			(quotedString, tokenID(ScriptTokenID::QuotedString))
			(number, tokenID(ScriptTokenID::Number))
		;
	}

	// the tokens expose their matched string as their parser attribute
	lex::token_def<> whitespace;
	lex::token_def<> newline;
	lex::token_def<std::string> identifier;
	lex::token_def<std::string> namespacedIdentifier;
	lex::token_def<std::string> blockContents;
	lex::token_def<std::string> verIdent;
	lex::token_def<> listBegin;
	lex::token_def<> listEnd;
	lex::token_def<> comma;
	lex::token_def<> equals;
	lex::token_def<> arrayBegin;
	lex::token_def<> arrayEnd;
	lex::token_def<> objectBegin;
	lex::token_def<> objectEnd;
	lex::token_def<> objectSeparator;
	lex::token_def<std::string> quotedString;
	lex::token_def<double> number;
};



// can't convert "Literal" to "Literal*"
// make special class?



namespace script_elements {

	struct ScriptVersion {
		int major;
		int minor;
		int patch;
	};
	
	struct Array;
	struct Object;


	struct Literal {
		boost::variant<double, std::string, boost::recursive_wrapper<Array>, boost::recursive_wrapper<Object>> contents;
	};

	struct Array {
		std::optional<Literal> first;
		std::vector<Literal> rest;
	};
	struct Object {
		std::optional<std::tuple<std::string, Literal>> first;
		std::vector<std::tuple<std::string, Literal>> rest;
	};

	
	struct List {
		std::optional<std::tuple<std::string, Literal>> first;
		std::vector <std::tuple<std::string, Literal>> rest;
	};


	using LiteralAdaptStructAlias = boost::variant<double, std::string, boost::recursive_wrapper<Array>, boost::recursive_wrapper<Object>>;
	using ObjectAdaptStructAlias = std::tuple<std::string, Literal>;
	using ListAdaptStructAlias = std::tuple<std::string, Literal>;
};
BOOST_FUSION_ADAPT_STRUCT(
	script_elements::ScriptVersion,
	(int, major)
	(int, minor)
	(int, patch)
);

BOOST_FUSION_ADAPT_STRUCT(
	script_elements::Literal,
	(script_elements::LiteralAdaptStructAlias, contents)
);

BOOST_FUSION_ADAPT_STRUCT(
	script_elements::Array,
	(std::optional<script_elements::Literal>, first)
	(std::vector<script_elements::Literal>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
	script_elements::Object,
	(std::optional<script_elements::ObjectAdaptStructAlias>, first)
	(std::vector<script_elements::ObjectAdaptStructAlias>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
	script_elements::List,
	(std::optional<script_elements::ListAdaptStructAlias>, first)
	(std::vector<script_elements::ListAdaptStructAlias>, rest)
)


template <typename Iterator, typename Skipper>
struct list_grammar : qi::grammar<Iterator, script_elements::List(), Skipper>
{
	template <typename TokenDef = script_tokens<lex::lexertl::lexer<>>>
	list_grammar(TokenDef const& tok)
	  : list_grammar::base_type(list, "ListGrammar")
	{	
		literal.name("Literal");
		array.name("Array");
		object.name("Object");
		list.name("List");
		name.name("Name (identifier)");

		literal %= tok.number | tok.quotedString | array | object;
		//array %= tok.arrayBegin >> -(literal) >> *(tok.comma >> literal) >> tok.arrayEnd;
		//object %= tok.objectBegin >> -(tok.identifier >> tok.objectSeparator >> literal) >> *(tok.comma >> tok.identifier >> tok.objectSeparator >> literal);
		name %= tok.identifier | tok.namespacedIdentifier;

		//list %= tok.listBegin >> -(name >> tok.equals >> literal) >> *(tok.comma >> name >> tok.equals >> literal) >> tok.listEnd;
	}
	qi::rule<Iterator, script_elements::Literal(), Skipper> literal;
	qi::rule<Iterator, script_elements::List(), Skipper> list;
	qi::rule<Iterator, script_elements::Object(), Skipper> object;
	qi::rule<Iterator, script_elements::Array(), Skipper> array;
	qi::rule<Iterator, std::string(), Skipper> name;
};



                                           

template <typename Iterator, typename Skipper>
qi::rule<Iterator, std::vector<int>, Skipper> version_identifier = "script version " >> qi::int_ % "." >> ";";




int main() {


	//lexer testing
	std::string contents = loadFileToString("sample.txt");
	const char* first = &*contents.begin();
	const char* last = &*contents.end();


	script_tokens<lex::lexertl::lexer<>> script_tokens_functor;

	
	std::vector<ScriptTokenID> tokenIds;
	std::vector<std::string> tokenContents;

	bool result = lex::tokenize(first, last, script_tokens_functor, [&](const auto& t){
		ScriptTokenID token = static_cast<ScriptTokenID>(t.id());

		tokenIds.push_back(token);

		std::string str{t.value().begin(), t.value().end()};

		tokenContents.push_back(str);
		return true;
	});

	using token_type = lex::lexertl::token<char const*, boost::mpl::vector<std::string>>;

  	using lexer_type = lex::lexertl::lexer<token_type>;

   	using iterator_type = script_tokens<lexer_type>::iterator_type;


	script_tokens<lexer_type> script_lex;
	list_grammar<iterator_type, decltype(script_lex.whitespace)> g (script_lex);
	
	first = &*contents.begin();
	last = &*contents.end();		
	
	result = lex::tokenize_and_phrase_parse(first, last, script_lex, g, script_lex.whitespace);
	std::cout << std::endl;

	if (result) {
		std::cout << "what? it worked?" << std::endl;
	}
	else {
		std::cout << "not surprising." << std::endl;

		std::cout << std::string{first, last} << std::endl;
	}
	
	
}