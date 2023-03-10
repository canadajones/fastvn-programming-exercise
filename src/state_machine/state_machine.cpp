#include <functional>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>
#include <ranges>
#include <cstdint>


#include <boost/unordered/unordered_flat_map.hpp>

namespace unordered = boost::unordered;

namespace vnpge {

namespace state_machine {


/**
 * @brief Represents the abstract state associated with a single moment in the story.
 * 
 */
class Page {
	unordered::unordered_flat_map<std::string, std::vector<char>> state;

	public:
	struct PageDataConstructor {};

	template <typename T>
	T retrieve(const std::string& key) requires requires {T{std::span(state.at(key)), PageDataConstructor{}};} {
		return T{std::span(state.at(key)), PageDataConstructor{}};
	}
};

/**
 * @brief Node in a DAG. Contains a Page.
 * NOT GOOD ENOUGH: needs further refinement of what makes a proper decider; must treat machine and user equally, and allow for the effective passing of named choices to a menu-like decider
 */
class GraphNode {
	std::vector<std::reference_wrapper<GraphNode>> nexts;
	Page page;

	public:

	explicit GraphNode(const Page& page) : page{page} {};
	explicit GraphNode(const Page&& page) : page{page} {};

	const Page& getPage() const {
		return page;
	}
};




} 
}