

#include "structures.h"

#include "video-sfml-text.h"

#include "video-sfml.h"

#include "video-sfml-compositor.h"

#include "compositor.h"


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

using namespace vnpge;

PositionedArea createTextBG(sf::RenderTarget& rt) {
	auto size = rt.getSize();

	sf::RectangleShape rectangle(sf::Vector2f(size.x, size.y));

	rectangle.setFillColor(sf::Color(0, 0, 0, 70));
	rectangle.setOutlineColor(sf::Color(255, 255, 255, 130));
	rectangle.setOutlineThickness(2);
	rt.draw(rectangle);

	return PositionedArea{ 
						.area     { .w = size.x - 8, 
									.h = size.x - 8
								  },
						.position { .x = 8,
									.y = 8
								  }
						};
}

int main() {

	SFMLWindow window;

	
	
	TextBox tb = {"sjomplern komplern stomplern", "assets/fonts/BonaNova-Italic.ttf", createTextBG};

	CompositorArea<SFMLRenderFunc> a{ {.dimensions = {  .w = 1.0,
										.h = 0.3 },
						.position = {.srcPos =  {.x = 0.5,
												 .y = 1.0 },
									 .destPos = {.x = 0.5,
												 .y = 1.0 }
									}
					  },
	[]{return true; }, [&tb](sf::RenderTarget& renderTarget) {
		tb.render(renderTarget);
	}};

	SFMLCompositor c{};
	c.addArea("text", "vnpge::ui", a);

	

	c.render(window.getWindow());

}
