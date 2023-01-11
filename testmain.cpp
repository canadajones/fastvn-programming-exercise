

#include "structures.h"

#include "video-sfml-text.h"

#include "video-sfml.h"

#include "video-sfml-compositor.h"

#include "compositor.h"

#include "debug.h"

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>


using namespace vnpge;

PositionedArea createTextBG(sf::RenderTarget& rt) {
	auto size = rt.getSize();

	sf::RectangleShape rectangle(sf::Vector2f(size.x-8, size.y-8));

	rectangle.setFillColor(sf::Color(0, 0, 0, 70));
	rectangle.setOutlineColor(sf::Color(255, 255, 255, 130));

	rectangle.setOutlineThickness(4);

	rt.draw(rectangle, sf::RenderStates(sf::BlendNone, sf::Transform().translate(4, 4), nullptr, nullptr));

	return PositionedArea{ 
						.area     { .w = size.x - 16, 
									.h = size.x - 16
								  },
						.position { .x = 8,
									.y = 8
								  }
						};
}


template <typename... Args>
void print(Args&&... args) {
	(std::cout <<  ... << args) << std::endl;
}

int main() {

	// Construct window
	SFMLWindow window;
	
	auto windowSize = window.getWindow().getSize();
	
	auto& sfWindow = window.getWindow();
	

	// Create textbox
	// TODO: delete or move this elsewhere
	TextBox tb = {"sjomplern komplern stomplern", "assets/fonts/BonaNova-Italic.ttf", {windowSize.x, windowSize.y / 3}, createTextBG};




	CompositorArea<SFMLRenderFunc> textBoxCompArea{ {  .dimensions = {  .w = 1.0,
																			  .h = 1.0/3.0 },
															.position	= {	.srcPos  = {.x = 0.5,
																					    .y = 0.5 },
																		  	.destPos = {.x = 0.5,
																						.y = 0.75 }
																		}
														},
									  []	 ()								  {return true; }, 
										  	[&tb](sf::RenderTarget& renderTarget) {tb.render(renderTarget);}
												};

	
	// Create compositor	
	SFMLCompositor compositor { {.w = windowSize.x, .h = windowSize.y}};


	compositor.addArea("text", "vnpge::ui", textBoxCompArea);






	// Print out size of window
	print("x: ", windowSize.x, ", y: ", windowSize.y);


	
 	while (sfWindow.isOpen()) {
	
		
		sfWindow.clear(sf::Color(70, 65, 140, 255));
		
		
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (sfWindow.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				sfWindow.close();
		}
		

		// Draw composite output to window
		
		compositor.render(sfWindow);


		window.getWindow().display();	
	}
}
