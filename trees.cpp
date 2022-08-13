#include <stdexcept>
#include <vector>
#include <queue>
#include <optional>
#include <string>
#include <functional>

#include <cassert>

#include "util.h"
#include "trees.h"

namespace trees {

	using ApplyFunction = std::function<void(const OrderNode& node)>;

	std::optional<std::reference_wrapper<const OrderNode>> dfs(const OrderNode& node, std::string_view id, ApplyFunction f) {
		
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
	}

	std::optional<std::reference_wrapper<const OrderNode>> bfs(const OrderNode& node, std::string_view id, ApplyFunction f) {
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
	}
	
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
			auto reversedLineage = r_wrap(lineage);
			lineages.emplace_back(reversedLineage.begin(), reversedLineage.end());
		}

		for (auto& lineage : lineages) {
			buildBranch(root, lineage);
		}
		
		return root;
	}
	
}