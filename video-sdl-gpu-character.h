#include <string>
#include <memory>
#include <unordered_map>

#include "image.h"


#ifndef STORYCHARACTER_HEADER
#define STORYCHARACTER_HEADER
namespace vnpge {
/**
 * @brief Template for creating Characters
 * Required to create Characters and MetaFrames, and by extension, Frames.
 */
struct MetaCharacter {
	// Credit to ThePotatoGuy for the name/idea of this scheme
	public:
		std::string id;
		
		std::string name;
		
		std::unordered_map<std::string, std::string> metaExpressions;
	public:
		/**
		 * @brief Construct a new MetaCharacter.
		 * Template for creating story characters.
		 * @param characterName Name of the character.
		 * @param metaExpressions An unordered map between expression codes and image paths.
		 * @param id A string uniquely identifying this (Meta)Character. Is used to link to (Meta)Frames.
		 */
		MetaCharacter(std::string characterName, const std::unordered_map<std::string, std::string>& metaExpressions, std::string id ) 
		: id(id), name{characterName}, metaExpressions{metaExpressions} {};


		MetaCharacter() = delete;

};


/**
 * @brief Main story character class
 * Contains everything needed to display a character.
 * NOTE: Requires a meta-character template to construct
 */
struct Character {
	public:
		std::string name;
		std::unordered_map<std::string, Image> expressions;
	public:
	/**
	 * @brief Construct a new Character.
	 * 
	 * @param metaCharacter The MetaCharacter template from which the name and expressions will be drawn
	 */
		Character(const MetaCharacter& metaCharacter) : name(metaCharacter.name) {
			for (auto& metaExpression : metaCharacter.metaExpressions) {
				expressions.insert({metaExpression.first,{metaExpression.second}});
			}
		};
		Character() = delete;
};
};
#endif