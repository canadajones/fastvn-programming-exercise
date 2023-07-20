#ifndef SCRIPTFLOW_PARSER_DEFINITION_HEADER
#define SCRIPTFLOW_PARSER_DEFINITION_HEADER



#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3.hpp>

#include "ast.h"
#include "ast_adapted.h"
#include "boost/spirit/home/x3/support/utility/annotate_on_success.hpp"
#include "boost/spirit/home/x3/support/utility/error_reporting.hpp"

#include "error_handler.h"
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
        struct annotation_tag;
        struct element_tag;

        x3::rule<identifier_tag, std::string> identifier = "identifier";

        x3::rule<speaker_tag, ast::Speaker> speaker = "speaker";

        x3::rule<statement_tag, ast::Statement> statement = "statement";

        x3::rule<annotation_tag, ast::Annotation> annotation = "annotation";

        x3::rule<element_tag, x3::variant<ast::Statement, ast::Annotation>> element = "element";

        dialogue_type dialogue = "dialogue";



        // Parser definitions
		const auto quoted_string = lexeme['"' >> *(char_ - '"') >> '"'];

        // standard programming identifier rules: letter first, then alphanumerics
        const auto identifier_def = lexeme[(x3::alpha | char_('_') | char_('$')) >> *(x3::alnum | char_('_') | char_('$'))];


        const auto speaker_def = (identifier | x3::string("@")) >> (- (',' > identifier));

        const auto statement_def = speaker > ":" > quoted_string > skip(x3::lit(' ') | '\t')[x3::eps >> x3::eol];

        const auto annotation_def = identifier >> ("(" > (identifier % ',')) > ")";

        const auto element_def = annotation | statement;

        const auto dialogue_def = +(element) > x3::eoi;

        BOOST_SPIRIT_DEFINE(identifier, speaker, statement, annotation, element, dialogue)
        
        
        struct identifier_tag : x3::annotate_on_success {};
        struct speaker_tag : x3::annotate_on_success {};
        struct statement_tag : x3::annotate_on_success {};
        struct annotation_tag : x3::annotate_on_success {};
        struct dialogue_tag : x3::annotate_on_success, error_handler_base {};

    }

    parser::dialogue_type getDialogueParser() {
        return parser::dialogue;
    }

}



#endif