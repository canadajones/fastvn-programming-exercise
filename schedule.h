#include <vector>
#include <iterator>
#include <iostream>

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

	Event(Action action, int data1 = 0, int data2 = 0) : action{action}, data1{data1}, data2{data2} {};

	Action getAction() {
		return action;
	}

	std::pair<int, int> getData() {
		return {data1, data2};
	}
};
template <typename T>
class Schedule {
	private:
	std::vector<T> elements;
	typename std::vector<T>::iterator it;
	public:
	Schedule(std::vector<T>& schedule_elements) : elements{schedule_elements}, it(elements.begin()) {std::cout << "hello" << std::endl;};

	void enqueue(T element) {
		elements.push_back(element);
		it = elements.begin();	
	}

	T get() {
		return *it;
	};

	bool next() {
		if (it == elements.end()) {
			return false;
		} 
		
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

	int length() {
		return elements.size();
	};
	

	
};


#endif