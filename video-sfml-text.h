#ifndef VNPGE_VIDEO_SFML_TEXT_HEADER
#define VNPGE_VIDEO_SFML_TEXT_HEADER


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
	SFMLFont(const std::string& fontPath);
	const sf::Font& getFont() const;
};



class SFMLFontStorage {
	private:
	std::unordered_map<std::string, SFMLFont> map;

	public:
	SFMLFontStorage() = default;
	
	const sf::Font& operator()(const std::string& fontName);
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
	TextBox(const std::string& dialogue, const std::string& fontName, std::function<PositionedArea(sf::RenderTarget&)> createTextBG);

	void render(sf::RenderTarget& target);

	
	void setWrappedString(const std::string& dialogueString);
};

}

#endif