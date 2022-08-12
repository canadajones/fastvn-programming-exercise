#include <SFML/Graphics/RenderWindow.hpp>
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



using SFMLRenderFunc = std::function<void(sf::RenderTarget& target)>;




// TODO: clarify the role of the SFMLCompositorArea such that its ownership of dimensions (relative or otherwise) make sense
// Does it belong to the compositor, or the rendering party?
// One should only know the dimensions its rendering, while the other needs to know proportions and absolute sizes.




namespace vnpge {

class SFMLWindow {
	private:
	sf::RenderWindow window;
	
	public:
	SFMLWindow() : window{sf::VideoMode(800, 600), "test1"} {};

	sf::RenderWindow& getWindow() {
		return window;
	}
};

}