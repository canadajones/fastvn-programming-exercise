#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>
#include <vector>
#include <unordered_map>
#include <string>



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
		target.display();
		sprite.setTexture(target.getTexture());
	};

	SFMLCompositorArea::SFMLCompositorArea(SFMLCompositorArea&& a) : area{ a.area } {}
	


	bool SFMLCompositorArea::shouldRender() const {
		return area.shouldRender();
	}
	
	void SFMLCompositorArea::render() {
		area.renderToTarget(target);
		target.display();
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
				auto& area = areas.find(id)->second;
				auto rectSize = area.getSprite().getTextureRect();
				sf::FloatRect r = {static_cast<float>(rectSize.left), static_cast<float>(rectSize.top), static_cast<float>(rectSize.width), static_cast<float>(rectSize.height)};
				
				sf::View view{r};

				auto getPos = [](RelativeArea relPos){
					
					auto x_k = relPos.position.destPos.x - (relPos.position.srcPos.x * relPos.dimensions.w);
					auto y_k = relPos.position.destPos.y - (relPos.position.srcPos.y * relPos.dimensions.h);
					return std::pair{x_k, y_k};
				};

				auto p = getPos(area.area.area);
				
				// 							x, y, w, h			
				auto a = sf::FloatRect(p.first, p.second, area.area.area.dimensions.w, area.area.area.dimensions.h);
				
				view.setViewport(a);

				dest.setView(view);
				dest.draw(area.getSprite());
				dest.setView(dest.getDefaultView());
			}
		}
		
	}
};