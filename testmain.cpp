

#include "structures.h"

#include "video-sfml-text.h"

#include "video-sfml.h"

#include "video-sfml-compositor.h"

#include "compositor.h"

#include "debug.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <unistd.h>

using namespace vnpge;

PositionedArea createTextBG(sf::RenderTarget& rt) {
	auto size = rt.getSize();

	std::cout << size.x << " , " << size.y << std::endl;
	
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
	auto windowSize = window.getWindow().getSize();
	
	auto& sfWindow = window.getWindow();
	
	TextBox tb = {"sjomplern komplern stomplern", "assets/fonts/BonaNova-Italic.ttf", {window.getWindow().getSize().x, window.getWindow().getSize().x}, createTextBG};

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

	SFMLCompositor c { {.w = windowSize.x, .h = windowSize.y}};
	c.addArea("text", "vnpge::ui", a);


	std::cout << windowSize.x << " , " << windowSize.y << std::endl;


 	while (sfWindow.isOpen()) {
		sfWindow.clear(sf::Color(255, 255, 255, 255));
		
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (sfWindow.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				sfWindow.close();
		}
		
		// render stuff
		c.render(sfWindow);
		
		//tb.render(sfWindow);

		window.getWindow().display();

		
	}
	

}
