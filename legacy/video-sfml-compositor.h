#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
#include <unordered_map>
#include <string>



#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "compositor.h"
#include "structures.h"
#include "trees.h"
#include "video-sfml.h"

namespace vnpge {
class SFMLCompositorArea {
	private:
	RelativeArea area;

	sf::RenderTexture target;
	sf::Sprite sprite;
	
	std::string foreignId;
	
	public:

	std::function<bool()> shouldRender;

	std::function<void(sf::RenderTarget&)> renderToTarget;
	SFMLCompositorArea(const RelativeArea& area, std::function<bool()> shouldRender, std::function<void(sf::RenderTarget&)> render) :
		 area{area}, shouldRender{shouldRender}, renderToTarget{render} {};
	
	SFMLCompositorArea(const CompositorArea<SFMLRenderFunc>& area, std::string_view fId, AbsoluteDimensions targetPixelDims);

	SFMLCompositorArea(SFMLCompositorArea&& a);
	
	SFMLCompositorArea(const SFMLCompositorArea&) = delete;

	std::string_view getForeignId() const;

	const sf::Sprite& getSprite() const;
};


class SFMLCompositor {
	private:
	std::unordered_map<std::string, SFMLCompositorArea> areas;

	// todo: fix this mess
	AbsoluteDimensions targetPixelDims;
	
	public:
	std::vector<std::string> order;

	
	public:

	SFMLCompositor(AbsoluteDimensions size) : targetPixelDims{size} {};

	void addArea(std::string_view id, std::string_view foreignId, const CompositorArea<SFMLRenderFunc>& area);

	void removeArea(const std::string& id);

	void computeOrder();

	void render(sf::RenderTarget& dest);
};	
};
