#include <boost/json/error.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

#include <fstream>
#include <sstream>

#include <boost/json.hpp>

#include "character.h"
#include "chapter.h"


#ifndef GENERIC_LOADER_HEADER
#define GENERIC_LOADER_HEADER



namespace vnpge {
	namespace json = boost::json;
class ChapterLoader {
	private:
	std::vector<std::string> paths;
	
	public:

};

class JSONLoader {
	private:
	std::vector<std::string> paths;
	
	public:
	JSONLoader(std::vector<std::string>& paths) : paths{paths} {
		if (paths.size() < 1) {
			throw std::runtime_error("No paths to JSON files were specified!");
		}
	}

	void loadChapter() {

		std::ifstream file;
		file.open(paths[0], std::ios::in);
		
		std::string inputJSONString;
		std::string inputLine;
		
		while (std::getline(file, inputLine)) {
			inputJSONString.append(inputLine + "\n");
		}

		json::error_code ec;
		json::value root = json::parse(inputJSONString, ec);
		std::vector<MetaCharacter> metaCharacters;
		
		std::string chapterName = root.as_object()["chapterName"].as_string().data();

		for (auto& character : root.as_object()["storyCharacters"].as_array()) {
			std::string name = character.as_object()["name"].as_string().data();
			std::string id = character.as_object()["id"].as_string().data();
			std::unordered_map<std::string, std::string> metaExpressions;
			for (auto& expression : character.as_object()["expressions"].as_object()) {
				metaExpressions.insert({expression.key().data(), expression.value().as_string().data()});
			}
			metaCharacters.emplace_back(name, metaExpressions, id);
		}

		std::vector<MetaFrame> metaFrames;
		for (auto& metaFrame : root.as_object()["storyFrames"].as_array()) {
			std::string textDialogue = metaFrame.as_object()["textDialogue"].as_string().data();
			std::string characterID  = metaFrame.as_object()["characterID" ].as_string().data();
			std::string expression   = metaFrame.as_object()["expression"  ].as_string().data()
			std::string background   = metaFrame.as_object()["background"  ].as_string().data();
			PositionMapping posMap;
			{	
				//														   src			 x/y
				double srcX =  metaFrame.as_object()["position"].as_array()[0].as_array()[0].as_double();
				double srcY =  metaFrame.as_object()["position"].as_array()[0].as_array()[1].as_double();
				
				//														  dest			 x/y
				double destX = metaFrame.as_object()["position"].as_array()[1].as_array()[0].as_double();
				double destY = metaFrame.as_object()["position"].as_array()[1].as_array()[1].as_double();
				
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

			metaFrames.emplace_back(textDialogue, characterID, posMap, background);

		}
	}
	// Get the latest version of Boost installed!
	// Then, find something to load files into a string
	// Lastly, parse the strings and construct them into a Chapter
};
}
#endif
