#include "metasyntax.h"
#include "metasyntax_def.h"
#include "config.h"
#include "ast.h"
#include <iterator>

namespace metasyntax { namespace parser
{
	BOOST_SPIRIT_INSTANTIATE(document_type, iterator_type, context_type)

}
namespace ascii = boost::spirit::x3::ascii;
std::optional<ast::Document> parseMetasyntax(const std::string& str, const std::string& file) {
	using metasyntax::parser::iterator_type;

	iterator_type start = str.cbegin();
	iterator_type end = str.cend();
	
	using metasyntax::parser::error_handler_type;
	using metasyntax::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, file); 

	const auto parser = x3::with<metasyntax::parser::error_handler_tag>(std::ref(error_handler))[metasyntax::getDocumentParser()];



	metasyntax::ast::Document document;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, document);

	if (!result) {
		std::cout << "Parsing incomplete, test halted." << std::endl;
		return std::nullopt;
	}
	else return document;
}
}

