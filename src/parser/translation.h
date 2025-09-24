#ifndef VNPGE_PARSER_TRANSLATION_HEADER
#define VNPGE_PARSER_TRANSLATION_HEADER
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>


#include "metasyntax/metasyntax.h"
#include "parser/metasyntax/ast.h"
#include "scriptflow/scriptflow.h"


#include "state_machine/state_machine.h"

namespace translation {

namespace ms = metasyntax::ast;

class LineDeclarationDecoder {};

class CharacterLDDecoder : LineDeclarationDecoder {
	private:

	public:

	
};

inline void decodeCharacter(const std::string& name, const ms::List& list) {

}

inline void decodeLocation(const std::string& name, const ms::List& list) {

}

inline void decodeScene(const std::string& name, const ms::List& list) {

}

// We kind of need to know in the Metasyntax parser what the hell these different entities mean: we can't create Pages without knowing what the scenes are,
// and Metasyntax doesn't know about scenes, only linedecls and blockdecls.
// Scriptflow can tease the pages out of the scene blockdecls, but to know to call Scriptflow, we need to register it for the Scene.
// If we're to go ahead with global, lineal, local page storage, we need Scriptflow to impose that as well.
// Also, we should probably well-define transitions into and out of the pages; it seems like an important aspect of a FSM.
// Is it worth going ahead with this structure?
// We technically have all we need to display something sensible to the screen, so why not go ahead with that first?


struct DecodingVisitor {
	using result_type = void;
	std::vector<std::string> knownNamespaces;

	std::unordered_map<std::string, void(*)(const std::string&, const ms::List&)> lineDecls;
	std::unordered_map<std::string, void*> blockDecls;

	void operator()(const metasyntax::ast::UsingDeclaration& use)  {
		std::cout << "Using: " << use.name << "\n";
		knownNamespaces.push_back(use.name);
	}

	void operator()(const metasyntax::ast::LineDeclaration& line) const {
		decltype(lineDecls)::mapped_type lineMeaning = nullptr;

		if (!lineDecls.contains(line.type.stringify())) {
			std::string lineNs;
			for (auto&& ns : knownNamespaces) {
				if (lineDecls.contains(ns + line.type.stringify())) {
					lineNs = ns;
				}
			}
			if (lineNs != "") {
				lineMeaning = lineDecls.at(lineNs + line.type.stringify());
			}
		} 
		else {
			lineMeaning = lineDecls.at(line.type.stringify());
		}

		if (lineMeaning == nullptr) {
			std::string err = "No such linedef known: ";
			throw std::runtime_error(err.append(line.type.stringify()));
		}
		else {
			lineMeaning(line.name, line.list);
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
		visitor.lineDecls.insert({"vnpge.character", &decodeCharacter});
		visitor.lineDecls.insert({"vnpge.location", &decodeCharacter});
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