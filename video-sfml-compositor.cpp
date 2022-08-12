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

	SFMLCompositorArea(const CompositorArea<SFMLRenderFunc>& area, std::string_view fId) : foreignId{fId}, area{ area } {

		area.renderToTarget(target);
		sprite.setTexture(target.getTexture());
	};

	SFMLCompositorArea(SFMLCompositorArea&& a) : area{ a.area } {}
	
	SFMLCompositorArea(const SFMLCompositorArea&) = delete;

	bool shouldRender() const {
		return area.shouldRender();
	}
	
	void render() {
		area.renderToTarget(target);
	};
	

	std::string_view getForeignId() const {
		return foreignId;
	};

	const sf::Sprite& getSprite() const {
		return sprite;
	}
};


class SFMLCompositor {
	private:
	std::unordered_map<std::string, SFMLCompositorArea> areas;
	
	std::vector<std::string_view> order;

	
	public:

	SFMLCompositor() {};

	void addArea(std::string_view id, std::string_view foreignId, const CompositorArea<SFMLRenderFunc>& area) {
		areas.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(area, foreignId));
		computeOrder();
	};

	void removeArea(const std::string& id) {
		areas.erase(id);
		computeOrder();
	};

	void computeOrder() {
		std::unordered_map<std::string_view, std::string_view> m;
		for (auto& p : areas) {
			m.emplace(p.first, p.second.getForeignId());
		}
		
		auto root = trees::buildTree(m, "vnpge::foreground");
		
		order.clear();
		auto& orderRef = order;
		bfs(root, "", [&orderRef](const trees::OrderNode& n) {orderRef.push_back(n.id);});
	}

	void render(sf::RenderTarget& dest) {
		for (auto& area : areas) {
			if (area.second.shouldRender()) {
				area.second.render();
			}
		}

		for (auto& id : order) {
			// ugly hack; there should be a workaround for this, but making it work first is top priority
			dest.draw((*areas.find(std::string{id})).second.getSprite());
		}


	}
	
};
};