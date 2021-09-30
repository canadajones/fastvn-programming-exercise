#ifndef JSON_LOADER_HEADER
#define JSON_LOADER_HEADER

#include <SDL2/SDL_render.h>
#include <boost/json/src.hpp>
#include <boost/json/error.hpp>
#include <boost/json/value.hpp>


#include "files.h"

#include "character.h"
#include "chapter.h"
#include "loader.h"

namespace vnpge {

namespace json = boost::json;

class JSONLoader : ChapterLoader {
	private:
	static std::vector<std::string> loadIndex(std::string indexPath) {

		std::vector<std::string> indexPaths;
		json::object rootObj;
		{
		json::value root;
		root = json::parse(loadFileToString(indexPath));
		rootObj = root.as_object();
		}
		for (auto& path : rootObj["index"].as_object()["json"].as_array()) {
			indexPaths.push_back(path.as_string().data());
		}
		return indexPaths;
	}

	public:
	JSONLoader(std::string indexPath) : ChapterLoader(loadIndex(indexPath)) {
		if (paths.size() < 1) {
			throw std::runtime_error("No paths to JSON files were specified!");
		}
	}

	

	Chapter loadChapter(uint w, uint h, std::shared_ptr<SDL_Renderer> renderer) {
		
		// TODO: make this loop through all paths instead of just picking the first one
		// This is simple to do, but would probably require nesting all of this function inside of a loop

		// TODO: Gracefully handle the exception that occurs when invalid JSON is passed in
		// Also, we could allow JSON extensions here, but for now strict compliance is the best option
		json::object rootObj;
		{
		json::value root;
		root = json::parse(loadFileToString(paths[0]));
		rootObj = root.as_object();
		}

		
		
		std::string chapterName = rootObj["chapterName"].as_string().data();
		
		// Extract the character information
		std::vector<MetaCharacter> metaCharacters;
		for (auto& character : rootObj["storyCharacters"].as_array()) {
			// Firstly, extract the string fields into actual named std::strings
			std::string name = character.as_object()["name"].as_string().data();
			std::string id   = character.as_object()["id"  ].as_string().data();

			// Then loop through the expressions, adding them to an unordered_map that gets passed to the MetaCharacter constructor
			std::unordered_map<std::string, std::string> metaExpressions;
			for (auto& expression : character.as_object()["expressions"].as_object()) {
				metaExpressions.insert({expression.key().data(), expression.value().as_string().data()});
			}
			metaCharacters.emplace_back(name, metaExpressions, id);
		}

		// Map between IDs and their defined MetaCharacters.
		// This is done because creating a MetaFrame requires a valid MetaCharacter to "bind" to.
		// However, in the JSON itself, only the ID is stored.
		// Therefore, this serves as a convenient reference.
		// As an added bonus, if a character ID is mistyped, trying to find the MetaCharacter will throw
		std::unordered_map<std::string, MetaCharacter&> charIDToMetaRefMap;
		for (auto& metaCharacter : metaCharacters) {
			charIDToMetaRefMap.insert({metaCharacter.id, metaCharacter});
		}

		// Create the final story frames
		std::vector<MetaFrame> metaFrames;
		for (auto& metaFrame : rootObj["storyFrames"].as_array()) {
			// Extract text fields
			std::string textDialogue = metaFrame.as_object()["textDialogue"].as_string().data();
			std::string characterID  = metaFrame.as_object()["characterID" ].as_string().data();
			std::string expression   = metaFrame.as_object()["expression"  ].as_string().data();
			std::string background   = metaFrame.as_object()["background"  ].as_string().data();
			
			// Ball the position field up into a position mapping
			PositionMapping posMap;
			{	
				json::value test = metaFrame.as_object()["position"].as_array()[0].as_array()[1];
				//																				   src			 x/y
				double srcX =  json::value_to<double> (metaFrame.as_object()["position"].as_array()[0].as_array()[0]);
				double srcY =  json::value_to<double> (metaFrame.as_object()["position"].as_array()[0].as_array()[1]);
				
				//														  						   dest			 x/y
				double destX = json::value_to<double> (metaFrame.as_object()["position"].as_array()[1].as_array()[0]);
				double destY = json::value_to<double> (metaFrame.as_object()["position"].as_array()[1].as_array()[1]);
				
				// Packin' boxes of doubles
				posMap = {
					.srcPos = {
						.x = srcX,
						.y = srcY
					},
					.destPos = {
						.x = destX,
						.y = destY
					}
				};
			}
			// This throws if any character IDs in the source JSON were misspelled
			metaFrames.emplace_back(textDialogue, charIDToMetaRefMap.at(characterID), expression, posMap, background);
		}
		return {chapterName, metaCharacters, metaFrames, {.w = w, .h = h}, "BonaNova-Italic.ttf", renderer};
	}
};
}

#endif