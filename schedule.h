#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER

#include <stdexcept>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>


#include "structures.h"

namespace vnpge {
enum struct Action : int {
	nothing,
	next_page,
	prev_page,
	scroll_up,
	scroll_down,

	window_resized,
	clean_exit = 137
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

/**
 * @brief Queue-like data structure to handle ordered streams of things like input events and render calls
 * 
 * @tparam T 
 */
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
		return *it;
	};

	bool next() {
		// If the iterator is at the end (normally only happens when elements is empty),
		// log an error and return false to signify that get() is undefined
		if (it == elements.end()) {
			std::cout << "An attempt was made to step past the end of the Schedule array!" << std::endl;
			return false;
		}
		
		it = std::next(it);
		return true;
	};

	bool prev() {
		// Log an error if trying to step before begin, and return false to signify that it failed
		// This is less serious than dereferencing end(), since begin() is defined.
		if (it == elements.begin()) {
			std::cout << "An attempt was made to step before the beginning of the Schedule array!" << std::endl;
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
	typename std::vector<T>::iterator begin() {
		return elements.begin();
	}
	typename std::vector<T>::iterator end() {
		return elements.end();
	}
};
};
#endif