#include <string>
#include <functional>

#include "structures.h"

namespace vnpge {
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

template<typename T>
using TextBGCreator = std::function<T (AbsoluteDimensions, RelativeDimensions)>;

};