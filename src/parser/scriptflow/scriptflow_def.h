#ifndef SCRIPTFLOW_PARSER_DEFINITION_HEADER
#define SCRIPTFLOW_PARSER_DEFINITION_HEADER



#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3.hpp>

#include "ast.h"
#include "ast_adapted.h"
#include "scriptflow.h"


namespace scriptflow {
    namespace parser {
        namespace x3 = boost::spirit::x3;

        using x3::char_;
        using x3::lexeme;
        using x3::skip;

        struct identifier_tag;
        struct speaker_tag;
        struct statement_tag;
        

        x3::rule<identifier_tag, std::string> identifier = "identifier";

        x3::rule<speaker_tag, ast::Speaker> speaker = "speaker";

        x3::rule<statement_tag, ast::Statement> statement = "statement";

        dialogue_type dialogue = "dialogue";



        // Parser definitions
		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];

        // standard programming identifier rules: letter first, then alphanumerics
        const auto identifier_def = lexeme[(x3::alpha | char_('_') | char_('$')) >> *(x3::alnum | char_('_') | char_('$'))];


        const auto speaker_def = identifier >> (- identifier);

        const auto statement_def = speaker > ":" > quoted_string > skip(x3::lit(' ') | '\t')[x3::eps >> x3::eol];

        const auto dialogue_def = statement % ',';

        BOOST_SPIRIT_DEFINE(identifier, speaker, statement, dialogue)


}}



#endif