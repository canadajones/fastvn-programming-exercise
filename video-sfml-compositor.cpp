#include <vector>
#include <unordered_map>
#include <string>



#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "compositor.h"
#include "structures.h"
#include "trees.h"
#include "video-sfml.h"
#include "debug.h"

#include "video-sfml-compositor.h"

namespace vnpge {

	SFMLCompositorArea::SFMLCompositorArea(const CompositorArea<SFMLRenderFunc>& area, std::string_view fId, AbsoluteDimensions targetPixelDims) : foreignId{fId}, area{ area } {

		target.create(area.area.dimensions.w * targetPixelDims.w, area.area.dimensions.h * targetPixelDims.h);
		area.renderToTarget(target);
		sprite.setTexture(target.getTexture());
	};

	SFMLCompositorArea::SFMLCompositorArea(SFMLCompositorArea&& a) : area{ a.area } {}
	


	bool SFMLCompositorArea::shouldRender() const {
		return area.shouldRender();
	}
	
	void SFMLCompositorArea::render() {
		area.renderToTarget(target);
	};
	

	std::string_view SFMLCompositorArea::getForeignId() const {
		return foreignId;
	};

	const sf::Sprite& SFMLCompositorArea::getSprite() const {
		return sprite;
	}

	
	void SFMLCompositor::addArea(std::string_view id, std::string_view foreignId, const CompositorArea<SFMLRenderFunc>& area) {
		areas.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(area, foreignId, targetPixelDims));
		computeOrder();
	};

	void SFMLCompositor::removeArea(const std::string& id) {
		areas.erase(id);
		computeOrder();
	};

	void SFMLCompositor::computeOrder() {
		std::unordered_map<std::string_view, std::string_view> m;
		for (auto& p : areas) {
			m.emplace(p.first, p.second.getForeignId());
		}
		
		auto root = trees::buildTree(m, "vnpge::ui");
		
		order.clear();
		auto& orderRef = order;
		bfs(root, "", [&orderRef](const trees::OrderNode& n) {orderRef.push_back(n.id);});

	}

	void SFMLCompositor::render(sf::RenderTarget& dest) {
		for (auto& area : areas) {
			if (area.second.shouldRender()) {
				area.second.render();
			}
		}

		for (auto& id : order) {
			// ugly hack; there should be a workaround for this, but making it work first is top priority

			if (id != "vnpge::ui") {
				dest.draw((*areas.find(id)).second.getSprite());
			}
		}
	}
	
};