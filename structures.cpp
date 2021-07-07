#include <SDL2/SDL_surface.h>
#include <string>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>


using std::string;
using std::vector;

class Image {
    private:
    string url;
    
    SDL_Surface imageSurface;

    SDL_Rect imagePosRect;

    public:
    Image(string location) {
        url = location;
    }
    Image(const Image& img) {
        url = img.url;

    }
    Image() = default;

    string getURL() {
        return url;
    }
    
};

class Character {
    private:
        string name;
        Image charImage;
    public:
        Character(string characterName, string imageURL) : name{characterName}, charImage{imageURL} {
        }
        Character() = default;
};

class Frame {
    private:
        string textDialogue;
        Character& storyCharacter;
    public:
    Frame(string dialogue, Character& character) : textDialogue{dialogue}, storyCharacter{character} {
    }

    Character& getCharacter() {
        return storyCharacter;
    }
};

class Chapter {
    private:
        string chapterName;
        vector<Character> storyCharacters;
        vector<Frame> storyFrames;

    public:
        Chapter(string name, vector<Character> characters, vector<Frame>& frames) : chapterName{name}, storyCharacters{characters}, storyFrames{frames} {
        }
};



Character m {"Monika", "test_char.png"};

Frame frameTest{"hello", m};

Character charTest = frameTest.getCharacter();