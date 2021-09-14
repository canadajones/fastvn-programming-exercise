# Comment out if needed; since I am the primary user of this file, this makes 100% sure that I use the correct compiler
#set(CMAKE_CXX_COMPILER "g++")

cmake_minimum_required(VERSION 3.0.0)
project(test1-vn VERSION 0.1.0 LANGUAGES CXX)

# This apparently may cause issues with non-makefile generators. Comment out if needed
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_VERBOSE_MAKEFILE TRUE)

add_executable(test1 main.cpp)

target_link_libraries(test1 SDL2 SDL2_image SDL2_ttf)

# Those sweet, sweet designated initialisers force me to use C++20
set_property(TARGET test1 PROPERTY CXX_STANDARD 20)
set_property(TARGET test1 PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET test1 PROPERTY CXX_STANDARD_REQUIRED ON)

if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(test1 PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter -pthread)
endif()
