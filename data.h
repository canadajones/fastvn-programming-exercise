#include <vector>
#include <string>
#include <iostream>


#include "structures.h"

#ifndef DATALOAD_HEADER
#define DATALOAD_HEADER

using namespace vnpge;
// NOLINTNEXTLINE(misc-definitions-in-headers)
Chapter initTest() {
	std::string dialogues[5] = {"Thank you for participating in this test.", "If you can read this, then it probably worked", "Well, that's all for now.", "Love you!", "Goodbye!"};

	Chapter main{"test", std::vector<Character>{Character("Eclaire", "eclaire2.png")}, std::vector<Frame>{}};

	Image bg{"lake.jpg"};
	for (uint64_t i = 0; i < 5; i++) {
		main.storyFrames.push_back(Frame(dialogues[i], main.storyCharacters[0], bg));
	}
	
	std::cout << "end_init_test" << std::endl;
	return main;
}

#endif
