module;
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

export module SFMLRendering;



export namespace trees {
	class OrderNode {
	public:
		std::string id = "root";

		std::vector<OrderNode> subNodes;

		OrderNode(const std::string_view id) : id{ id } {};

	};


	using ApplyFunction = std::function<void(const OrderNode& node)>;

	std::optional<std::reference_wrapper<const OrderNode>> dfs(const OrderNode& node, std::string_view id, ApplyFunction f = [](auto){}) {
		
		f(node);
		if (node.id == id) {
			return node;
		}

		for (auto& subNode : node.subNodes) {	
			if (auto result = dfs(subNode, id, f)) {
				return result;
			}
		}
		return std::nullopt;
	};

	std::optional<std::reference_wrapper<const OrderNode>> bfs(const OrderNode& node, std::string_view id, ApplyFunction f = [](auto) {}) {
		std::queue<std::reference_wrapper<const OrderNode>> q{};

		q.push(node);
		while (!q.empty()) {
			auto n = q.front();
			q.pop();

			f(n);
			
			if (n.get().id == id) {
				return n;
			}
			for (auto& m : n.get().subNodes) {
				q.push(m);
			}
		}
		return std::nullopt;
	};
	
	std::string prettifyTree(OrderNode& root) {
		std::string retval{""};
		dfs(root, "", [&retval](const OrderNode& n) {
			if (n.subNodes.size() > 0) {
				retval.append("[");
				retval.append(n.id);
				retval.append("] - (");

				for (auto m = n.subNodes.cbegin(); m != n.subNodes.cend(); ++m) {
					retval.append("'");
					retval.append(m->id);
					if (std::next(m) != n.subNodes.cend()) {
						retval.append("', ");
					}
					else {
						retval.append("' ");
					}
				}
				retval.append("children) -> ");
			}
			else {
				retval.append("(");
				retval.append(n.id);
				retval.append(")\n");
			}
			});
		return retval;
	}


	template <std::ranges::bidirectional_range StringRange>
	void buildBranch(OrderNode& node, StringRange lineage) {
		if (lineage.size() == 0) {
			throw std::logic_error("Cannot build branch with null lineage vector.");
		}
		

		OrderNode* currentNode = &node;
		for (auto id = lineage.begin(); id != lineage.end(); ++id) {
			if (currentNode->id == *id) {
				continue;
			}
			auto it = std::find_if(currentNode->subNodes.begin(), currentNode->subNodes.end(),
									[&id](const OrderNode& n) {return n.id == *id;});

			
			if (it == currentNode->subNodes.end()) {
				for (; id != lineage.end(); ++id) {
					currentNode = &(currentNode->subNodes.emplace_back(*id));
				}
				
				return;
			}
			else {
				currentNode = &(*it);
			}

		}
	}
	
	OrderNode buildTree(const std::unordered_map<std::string_view, std::string_view>& m, std::string_view rootId) {
		OrderNode root{rootId};
		
		std::vector<std::vector<std::string_view>> lineages;
		for (auto& p : m) {
			std::vector<std::string_view> lineage;
			
			std::string_view curId = p.first;
			while (true) {
				lineage.push_back(curId);
				if (curId == rootId) {
					break;
				}
				curId = m.at(curId);
			}
			auto reversedLineage = lineage | std::views::reverse;
			lineages.emplace_back(reversedLineage.begin(), reversedLineage.end());
		}

		for (auto& lineage : lineages) {
			buildBranch(root, lineage);
		}
		
		return root;
	}
	
}
using RenderFunc = std::function<void(sf::RenderTarget& target)>;

class Order {
	public:
	std::string id;
	std::string foreignId;

	Order(std::string_view id, std::string_view foreignId) : id{ id }, foreignId{ foreignId } {};
};



// TODO: clarify the role of the SFMLCompositorArea such that its ownership of dimensions (relative or otherwise) make sense
// Does it belong to the compositor, or the rendering party?
// One should only know the dimensions its rendering, while the other needs to know proportions and absolute sizes.





export namespace vnpge {

class CompositorArea {
	public:
	RelativeArea area;

	std::function<bool()> shouldRender;

	RenderFunc renderToTarget;

	CompositorArea(const RelativeArea& area, std::function<bool()> shouldRender, RenderFunc render) :
		 area{area}, shouldRender{shouldRender}, renderToTarget{render} {};
};

class SFMLCompositorArea {
	private:
	sf::RenderTexture target;
	std::string foreignId;

	CompositorArea area;

	public:

	SFMLCompositorArea(const CompositorArea& area, std::string_view fId) : area{ area }, foreignId{fId} {};

	SFMLCompositorArea(SFMLCompositorArea&& a) : area{ a.area } {}
	
	SFMLCompositorArea(const SFMLCompositorArea&) = delete;

	void render() {
		if (area.shouldRender()) {
			area.renderToTarget(target);
		}
	};

	std::string_view getForeignId() const {
		return foreignId;
	};
};



class SFMLCompositor {
	private:
	std::unordered_map<std::string, SFMLCompositorArea> areas;
	
	std::vector<std::string_view> order;

	
	public:

	SFMLCompositor() {};

	void addArea(std::string_view id, std::string_view foreignId, const CompositorArea& area) {
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


};

class SFMLWindow {
	sf::RenderWindow window;
};

}