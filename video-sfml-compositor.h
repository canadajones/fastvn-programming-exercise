#include <vector>
#include <unordered_map>
#include <string>



#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "compositor.h"
#include "trees.h"
#include "video-sfml.h"

namespace vnpge {
class SFMLCompositorArea {
	private:
	sf::RenderTexture target;
	sf::Sprite sprite;
	std::string foreignId;

	CompositorArea<SFMLRenderFunc> area;

	public:

	SFMLCompositorArea(const CompositorArea<SFMLRenderFunc>& area, std::string_view fId);

	SFMLCompositorArea(SFMLCompositorArea&& a);
	
	SFMLCompositorArea(const SFMLCompositorArea&) = delete;

	bool shouldRender();
	
	void render();
	

	std::string_view getForeignId() const;

	const sf::Sprite& getSprite() const;
};


class SFMLCompositor {
	private:
	std::unordered_map<std::string, SFMLCompositorArea> areas;
	
	std::vector<std::string_view> order;

	
	public:

	SFMLCompositor() = default;

	void addArea(std::string_view id, std::string_view foreignId, const CompositorArea<SFMLRenderFunc>& area);

	void removeArea(const std::string& id);

	void computeOrder();

	void render(sf::RenderTarget& dest);
};	
};
