#ifndef VNPGE_PARSER_TRANSLATION_HEADER
#define VNPGE_PARSER_TRANSLATION_HEADER
#include <cstdint>
#include <unordered_map>


#include "metasyntax/metasyntax.h"
#include "parser/metasyntax/ast.h"
#include "scriptflow/scriptflow.h"


#include "state_machine/state_machine.h"

namespace translation {

namespace ms = metasyntax::ast;


struct DecodingVisitor {
	using result_type = void;
	std::vector<std::string> knownNamespaces;

	std::unordered_map<std::string, void*> lineDefs;
	std::unordered_map<std::string, void*> blockDefs;

	void operator()(const metasyntax::ast::UsingDeclaration& use)  {
		std::cout << "Using: " << use.name << "\n";
		knownNamespaces.push_back(use.name);
	}

	void operator()(const metasyntax::ast::LineDeclaration& line) const {
		std::cout << "Line: " << line.type.stringify() << " " << line.name << "\n";
		std::cout << "\tList: ";

		if (lineDefs.contains(line.type.stringify())) {

		}


	}
	void operator()(const metasyntax::ast::BlockDeclaration& block) const {
		std::cout << "Line: " << block.type.stringify() << " " << block.name << "\n";
		std::cout << "\tList: ";
		std::cout << "\n Block: " << block.block << std::endl;	
	}
};


inline void begin_peel(ms::Document onion) {
	

	if (onion.version == ms::DocumentVersion{.major = 0, .minor = 0, .patch = 1}) {
		DecodingVisitor visitor;
		for (auto& decl : onion.contents) {
			boost::apply_visitor(visitor, decl);

		}
		for (auto&& ns : visitor.knownNamespaces) {
			std::cout << ns << "\n";
		}
	}   
}




}
#endif