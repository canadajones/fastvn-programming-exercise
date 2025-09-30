
#include "parser/metasyntax/config.h"
#include "parser/metasyntax/metasyntax.h"
#include "parser/scriptflow/scriptflow.h"
#include "parser/translation.h"
#include "gfx/window/window.h"
#include "util/files.h"

namespace x3 = boost::spirit::x3;
namespace window = vnpge::gfx::window;
namespace ascii = boost::spirit::x3::ascii;


int main(int argc, char** argv)  {
    // init stuff
    window::SDLScope scope;
    window::Window window;
    window::Renderer renderer{window};
    // load file
    
    std::string file = "metasyntax.sample.txt";
	std::string test = loadFileToString(file);

    // parse file
	// todo: create dedicated parser function
	using metasyntax::parser::iterator_type;

	iterator_type start = test.cbegin();
	iterator_type end = test.cend();
	
	using metasyntax::parser::error_handler_type;
	using metasyntax::parser::error_handler_tag;
	error_handler_type error_handler(start, end, std::cout, file); 

	const auto parser = x3::with<metasyntax::parser::error_handler_tag>(std::ref(error_handler))[metasyntax::getDocumentParser()];



	metasyntax::ast::Document document;
	auto result = x3::phrase_parse(start, end, parser, ascii::space, document);

	if (!result) {
		std::cout << "Parsing incomplete, test halted." << std::endl;
		return -1;
	}
    
   
    
    // create usable text from file contents
    // display text on screen
    // handle sdl event loop




}