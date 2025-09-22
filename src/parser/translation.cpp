
#include <cstdint>
#include <unordered_map>


#include "metasyntax/metasyntax.h"
#include "parser/metasyntax/ast.h"
#include "scriptflow/scriptflow.h"


#include "state_machine/state_machine.h"

namespace translation {

namespace ms = metasyntax::ast;

struct decoding_visitor {
	using result_type = void;

	void operator()(const metasyntax::ast::UsingDeclaration& use) const {
		std::cout << "Using: " << use.name << "\n";
	}

	void operator()(const metasyntax::ast::LineDeclaration& line) const {
		std::cout << "Line: " << line.type.stringify() << " " << line.name << "\n";
		std::cout << "\tList: ";

	}
	void operator()(const metasyntax::ast::BlockDeclaration& block) const {
		std::cout << "Line: " << block.type.stringify() << " " << block.name << "\n";
		std::cout << "\tList: ";
		std::cout << "\n Block: " << block.block << std::endl;	
	}
};


void begin_peel(ms::Document onion) {
    if (onion.version == ms::DocumentVersion{.major = 0, .minor = 0, .patch = 1}) {
        decoding_visitor visitor;
        for (auto& decl : onion.contents) {
            boost::apply_visitor(visitor, decl);

        }
    }   
}




}