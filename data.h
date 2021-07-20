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


	TextBox textBox = {{.w = w, .h = h}, "BonaNova-Italic.ttf", makeTextBox};

	std::vector<MetaCharacter> metaCharacters = {{"Eclaire", "eclaire2.png", "eclaire-0"}};
	std::vector<MetaFrame> metaFrames = {};

	Image bg{"lake.jpg"};

	for (uint64_t i = 0; i < 5; i++) {
		metaFrames.emplace_back(dialogues[i], metaCharacters[0], bg);
	}

	Chapter main{"test", metaCharacters, metaFrames, {.w = w, .h = h}, "BonaNova-Italic.ttf", makeTextBox};

	
	
	
	std::cout << "end_init_test" << std::endl;
	return main;
}

#endif
