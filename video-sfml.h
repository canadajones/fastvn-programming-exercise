#ifndef VNPGE_VIDEO_SFML_HEADER
#define VNPGE_VIDEO_SFML_HEADER


#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <exception>
#include <optional>
#include <queue>
#include <ranges>
#include <utility>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "structures.h"
#include "util.h"


using SFMLRenderFunc = std::function<void(sf::RenderTarget&)>;



namespace vnpge {

class SFMLWindow {
	private:
	sf::RenderWindow window;
	
	public:
	SFMLWindow(){}

	sf::RenderWindow& getWindow();
};

}

#endif