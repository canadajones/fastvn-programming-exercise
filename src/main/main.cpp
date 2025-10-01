
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
	
    
   
    
    // create usable text from file contents
    // display text on screen
    // handle sdl event loop




}