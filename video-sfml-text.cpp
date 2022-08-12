#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

#include "structures.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>


namespace vnpge {
class SFMLFont {
	private:
	std::string fontName;
	sf::Font font;
	
	public:
	SFMLFont(const std::string& fontPath) : fontName{fontPath} {
		if (!font.loadFromFile(fontPath)) {
			std::string err = "Error: Font could not be loaded. Missing font file path: ";
			throw std::runtime_error(err.append(fontPath));
		}
	}
	const sf::Font& getFont() const {
		return font;
	}
};



class SFMLFontStorage {
	private:
	std::unordered_map<std::string, SFMLFont> map;

	public:
	SFMLFontStorage() = default;
	
	const sf::Font& operator()(const std::string& fontName) {
		if (!map.contains(fontName)) {
			map.insert({fontName, SFMLFont{fontName}});
		}
		return map.at(fontName).getFont();
	}
};


class TextBox {
	private:
	SFMLFontStorage fontStorage;

	sf::Text text;

	sf::RenderTexture background;
	sf::Sprite bgSprite;

	
	PositionedArea validTextSpace;

	std::function<PositionedArea(sf::RenderTarget&)> createTextBG;
	public:
	TextBox(const std::string& dialogue, const std::string& fontName, std::function<PositionedArea(sf::RenderTarget&)> createTextBG) {
		
		text.setFont(fontStorage(fontName));
		text.setStyle(sf::Text::Regular);
		
		// TODO: implement pixel size selection algorithm and story frame dependence
		text.setCharacterSize(24);

		// TODO: make this dependent on story frame
		text.setFillColor(sf::Color(255, 255, 255, 255));
		
		auto area = createTextBG(background);
		
		text.setPosition(area.position.x, area.position.y);
		
		setWrappedString(dialogue);

		bgSprite.setTexture(background.getTexture());
	}

	void render(sf::RenderTarget& target) {

		target.draw(bgSprite);
		target.draw(text);

	}


	
	void setWrappedString(const std::string& dialogueString) {
		
		
		std::vector<uint> spacePositions;

		
		for (auto it = dialogueString.begin(); it != dialogueString.end(); it++) {
			if (*it == ' ') {
				spacePositions.push_back(std::distance(dialogueString.begin(), it));
			}
		}

		std::string wrappedString = dialogueString;

		for (auto spacePos : spacePositions) {
			text.setString(wrappedString);

			auto charPos = text.findCharacterPos(spacePos);
			
			// BUG: this interacts poorly with multiple consecutive spaces
			// TODO: test if this is true
			if (charPos.x > validTextSpace.position.x + validTextSpace.area.w) {
				wrappedString[spacePos] = '\n';
			}
		}
	}
};


}

