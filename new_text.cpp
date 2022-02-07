module;

#include "structures.h"
#include <string>
#include <functional>

export module StoryDialogue;

export namespace vnpge {
	
class DialogueFont {
	private:
	std::string name;
	
	public:	

	// TODO: put in getPtSize here from the other file
	DialogueFont(std::string path) : name{path} {};
	
	std::string getName() {
		return name;
	}
};

class Dialogue {
	private:
	std::string name;
	std::string text;
	Colour colour;
	public:

	Dialogue(std::string name, std::string text, Colour colour) : name{name}, text{text}, colour{colour} {};

	
	std::string getName() const {
		return name;
	}

	std::string getText() const {
		return text;
	}

	Colour getColour() const {
		return colour;
	}
};

//														  Usable text area	   Screen size	   Text box area relative to screen size
template<typename Renderable>
using TextBGCreator = std::function<std::pair<Renderable, PositionedArea> (AbsoluteDimensions, RelativeDimensions)>;

};