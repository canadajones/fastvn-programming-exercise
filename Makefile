#SOURCE specifies which files to compile as part of the project
SOURCE = main.cpp

#CC specifies which compiler we're using
COMPILER = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -O2 -Wall -Werror -pedantic -std=c++17

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image

#EXe_NAME specifies the name of our executable
EXE_NAME = test1

#This is the target that compiles our executable
all : $(SOURCE)
	$(COMPILER) $(SOURCE) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(EXE_NAME)

debug : $(SOURCE)
	$(COMPILER) $(SOURCE) $(COMPILER_FLAGS) $(LINKER_FLAGS) -ggdb -o $(EXE_NAME)

windows : $(SOURCE)
	x86_64-w64-mingw32-g++ $(SOURCE) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(EXE_NAME).exe
