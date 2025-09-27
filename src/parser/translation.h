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

inline void decodeCharacter(const std::string& name, const ms::List& list);
inline void decodeLocation(const std::string& name, const ms::List& list);
inline void decodeScene(const std::string& name, const ms::List& list);


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


void begin_peel(ms::Document onion);



}
#endif