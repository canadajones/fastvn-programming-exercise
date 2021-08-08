#include <vector>
#include <string>
#include <iostream>


#include "structures.h"

#ifndef DATALOAD_HEADER
#define DATALOAD_HEADER

using namespace vnpge;

// NOLINTNEXTLINE(misc-definitions-in-headers)
SDL_Surface* makeTextBox(AbsoluteDimensions pixelDimensions, RelativeDimensions relDimensions) {
	// Default simple text box
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, pixelDimensions.w*relDimensions.w, pixelDimensions.h*relDimensions.h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	#else
	SDL_Surface* textBoxSurface = SDL_CreateRGBSurface(0, pixelDimensions.w*relDimensions.w, pixelDimensions.h*relDimensions.h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	#endif
	
	if (textBoxSurface == nullptr) {
		std::string err = "Surface could not be created! SDL_Error: "; 
		throw std::runtime_error(err.append(SDL_GetError()));
	}

	SDL_Rect textBox = {
		.x = 0,
		.y = 0,
		.w = textBoxSurface->w,
		.h = textBoxSurface->h
	};

	// Fill with a transparent white
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0xFF, 0xFF, 0xFF, 0x7F));

	textBox.x += 4;
	textBox.y += 4;
	textBox.w -= 8;
	textBox.h -= 8;

	// Fill with a transparent black inside the other rectangle, thereby creating a white border
	// Note that FillRect does not blend alphas, so this alpha replaces the white's
	// This is actually desirable, since the colours should have different alphas in order to feel equally transparent
	SDL_FillRect(textBoxSurface, &textBox, SDL_MapRGBA(textBoxSurface->format, 0x30, 0x30, 0x30, 0xAF));

	return textBoxSurface;
};
// NOLINTNEXTLINE(misc-definitions-in-headers)
Chapter initTest(uint w, uint h) {
	std::string dialogues[5] = {"Thank you for participating in this test.", "If you can read this, then it probably worked", "Well, that's all for now.", "Love you!", "Goodbye!"};

	std::vector<std::string> dialogue2 = {"Welcome to the extended text portion of this test program.",
											"Text in this portion will be excessively verbose such that it can properly " //NOLINT(bugprone-suspicious-missing-comma
											"test the capabilities of the text splitter and reassembler.",
											"Hmm. what if I threw in a \\n? That ought\nto cause some havoc. It'll "
											"at least tell us if line feeds require special treatment. I wonder if "
											"they would make for a good way to break the text on the screen, as opposed "
											"to manually cutting things.",
											"Are things working? I hope so; it would be a shame if it didn't.",
											"Well, it'd be informative either way. That's how science works, if you didn't "
											"know. The point isn't to find that which will prove your hypothesis, but more "
											"so to learn what is actually true. If the truth means your hypothesis, "
											"no matter how exciting, is bust, then it's still a success."};

	TextBox textBox = {{.w = w, .h = h}, "BonaNova-Italic.ttf", makeTextBox};

	std::vector<MetaCharacter> metaCharacters = {{"Eclaire", {{"eclaire-neutral", "eclaire-neutral.png"}, {"eclaire-happy", "eclaire-happy.png"}}, "eclaire-0"},
												 {"Carl", {{"carl-neutral", "carl-neutral.png"}}, "carl-0"}};
	std::vector<MetaFrame> metaFrames = {};
	PositionMapping posMap = {
		.srcPos = {0.5, 1},
		.destPos = {0.5, 1},
	};
	for (uint64_t i = 0; i < 2; i++) {
		metaFrames.emplace_back(dialogues[i], metaCharacters[0], "eclaire-neutral", posMap, "lake.jpg");
	}
	posMap.srcPos.x = 0.25;
	posMap.destPos.x = 0.25;
	for (uint64_t i = 2; i < 5; i++) {
		metaFrames.emplace_back(dialogues[i], metaCharacters[0], "eclaire-happy", posMap, "lake2.jpg");
	}
	posMap.srcPos.x = 0.4;
	posMap.destPos.x = 0.5;
	for (auto& text : dialogue2) {
		metaFrames.emplace_back(text, metaCharacters[1], "carl-neutral", posMap, "lab.jpg");
	}

	Chapter main = {"test", metaCharacters, metaFrames, {.w = w, .h = h}, "BonaNova-Italic.ttf", makeTextBox};

	
	
	
	std::cout << "end_init_test" << std::endl;
	return main;
}

#endif
