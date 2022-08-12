#ifndef VNPGE_TREE_HEADER
#define VNPGE_TREE_HEADER

#include <stdexcept>
#include <vector>
#include <queue>
#include <optional>
#include <string>
#include <functional>
#include <concepts>

#include <cassert>

#include "util.h"

namespace trees {
	class OrderNode {
	public:
		std::string id = "root";

		std::vector<OrderNode> subNodes;

		OrderNode(const std::string_view id);

	};


	using ApplyFunction = std::function<void(const OrderNode& node)>;

	std::optional<std::reference_wrapper<const OrderNode>> dfs(const OrderNode& node, std::string_view id, ApplyFunction f = [](auto){});

	std::optional<std::reference_wrapper<const OrderNode>> bfs(const OrderNode& node, std::string_view id, ApplyFunction f = [](auto) {});
	
	std::string prettifyTree(OrderNode& root);


	template <std::ranges::bidirectional_range StringRange>
	void buildBranch(OrderNode& node, StringRange& lineage);
	
	OrderNode buildTree(const std::unordered_map<std::string_view, std::string_view>& m, std::string_view rootId);
	
}

#endif