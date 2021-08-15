#include <string>
#include <vector>
#include <iterator>
#include <iostream>

#include "structures.h"

#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER

enum struct Action : int {
	clean_exit,
	next_page,
	prev_page,
	scroll_up,
	scroll_down,

	window_resized
};

class Event {
	private:
	Action action;

	int data1;
	int data2;
	public:

	Event(Action action, int data1 = 255, int data2 = 255) : action{action}, data1{data1}, data2{data2} {};

	Action getAction() {
		return action;
	}

	std::pair<int, int> getData() {
		return {data1, data2};
	}
};
//NOLINTNEXTLINE
bool isIterEqual(std::vector<Event>::iterator it1, std::vector<Event>::iterator it2) {
	return it1 == it2;
};

std::string iterPrint(std::vector<Event>::iterator it) {
	std::string eventName = "";
	switch (it->getAction()) {
		case Action::clean_exit: {
			eventName.append("Action::clean_exit");
		}
		break;
		case Action::next_page:{ 
			eventName.append("Action::next_page");
		}
		break;
		case Action::prev_page: {
			eventName.append("Action::prev_page");
		}
		break;
		case Action::scroll_up: {
			eventName.append("Action::scroll_up");
		}
		break;
		case Action::scroll_down: {
			eventName.append("Action::scroll_down");
		}
		break;

		case Action::window_resized: {
			eventName.append("Action::window_resized");
		}
		break;
		

		
	}
	std::string data1 = std::to_string(it->getData().first);
	std::string data2 = std::to_string(it->getData().second);
	return "{ Action=" + eventName + ", data1=" + data1 + ", data2=" + data2 + " };";
}

template <typename T>
class Schedule {
	private:

	std::vector<T> elements;
	typename std::vector<T>::iterator it;

	public:

	Schedule(std::vector<T>& schedule_elements) : elements{schedule_elements}, it(elements.begin()) {};

	void enqueue(T element) {
		elements.push_back(element);
		it = elements.begin();	
	}

	T get() {
		std::cout << iterPrint(it) << std::endl;
		return *it;
	};

	bool next() {
		// If the iterator is at the end (normally only happens when elements is empty),
		// return with false to signal that there are no more valid elements, and that get() is undefined
		if (it == elements.end()) {
			return false;
		}
		// After this point, it's safe to assume that there is at least one element in elements, and that begin() != end()
		
		it = std::next(it);
		return true;
	};

	bool prev() {
		if (it == elements.begin()) {
			return false;
		} 
		
		it = std::prev(it);
		return true;
	};

	bool isValid() {
		// This is more of a dereferencability check, but for the purposes of the Schedule, they're the same.
		// Next() and prev() should prevent the iterator from going out of bounds.
		// Thus, we only need to check if we're at end().
		
		if (it == elements.end()) {
			return false;
		}
		// Since it is not at this point, the iterator should be dereferencible (valid).
		return true;
	}
	int length() {
		return elements.size();
	};
	
	void updateIterator() {
		it = elements.begin();
		std::cout << "upd" << iterPrint(it) << std::endl;
	}
};





#endif