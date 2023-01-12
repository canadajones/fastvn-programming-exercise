module;
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>



#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "structures.h"
#include "util.h"

export module SFMLRendering;

struct Order {
	public:
	std::string id;
	std::string foreignId;
	bool isBefore = false;
};


class OrderNode {
	public:
	std::string id;

	std::vector<OrderNode> subNodes;
	
	OrderNode(const std::string& id) : id{ id } {};

};

class RootNode {
	public:
	std::vector<OrderNode> subNodes;
};

using RenderFunc = std::function<void(sf::RenderTarget& target)>;



void recurseNode(const OrderNode& node, auto& it, const auto& end) {
	if (it == end) {
		return;
	}

	for (auto& node : node.subNodes) {
		if (node.id == *it) {
			recurseNode(node, ++it, end);
			return;
		}
	}
}

// TODO: clarify the role of the SFMLCompositorArea such that its ownership of dimensions (relative or otherwise) make sense
// Does it belong to the compositor, or the rendering party?
// One should only know the dimensions its rendering, while the other needs to know proportions and absolute sizes.





export namespace vnpge {

class CompositorArea {
	public:
	Order order;

	RelativeArea area;

	std::function<bool()> shouldRender;

	RenderFunc renderToTarget;

	CompositorArea(const Order& order, const RelativeArea& area,
		const PositionMapping& position, std::function<bool()> shouldRender, RenderFunc render) :
		order{ order }, area{ area }, shouldRender{ shouldRender }, renderToTarget{ render } {};
};

class SFMLCompositorArea {
	private:
	sf::RenderTexture target;

	CompositorArea area;

	public:

	SFMLCompositorArea(const CompositorArea& area) : area{area} {};

	void render() {
		if (area.shouldRender()) {
			area.renderToTarget(target);
		}
	}

	const Order& getOrder() {
		return area.order;
	}
	
};

class SFMLCompositor {
	private:
	std::unordered_map<std::string, SFMLCompositorArea> areas;

	RootNode areaTree;
	
	
	public:

	SFMLCompositor() {};

	void addArea(const std::string& id, const CompositorArea& area) {
		areas.emplace(std::make_pair(id, SFMLCompositorArea(area)));
	};

	void removeArea(const std::string& id) {
		areas.erase(id);
	};

	void computeOrder() {

		for (auto& areaPair : areas) {
			
		}
	}

	SFMLCompositorArea& traverseTree(std::string id) {
	
		RootNode root;
		std::vector<std::string> lineage;
		std::string currentId = id;
	
		while (true) {
			if (areas.at(currentId).getOrder().foreignId == "background") {
				break;
			}
			lineage.push_back(currentId);
			currentId = areas.at(currentId).getOrder().foreignId;
		}

		std::vector<std::string>::reverse_iterator it = lineage.rbegin();
	
		for (auto node : root.subNodes) {
			if (node.id == *it) {
				recurseNode(node, it, lineage.rend());
			}
	
	
		};
	
	}
};

class SFMLWindow {
	sf::RenderWindow window;
};

}