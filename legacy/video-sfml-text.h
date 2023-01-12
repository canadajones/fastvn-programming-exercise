#ifndef VNPGE_VIDEO_SFML_TEXT_HEADER
#define VNPGE_VIDEO_SFML_TEXT_HEADER


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
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

	public:
	sf::RenderTexture background;
	sf::Sprite bgSprite;

	
	PositionedArea validTextSpace;

	std::function<PositionedArea(sf::RenderTarget&)> createTextBG;
	public:
	TextBox(const std::string& dialogue, const std::string& fontName, std::pair<uint, uint> size, std::function<PositionedArea(sf::RenderTarget&)> createTextBG);

	void render(sf::RenderTarget& target);

	
	void setWrappedString(const std::string& dialogueString);
};

}

#endif