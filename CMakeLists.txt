cmake_minimum_required(VERSION 3.0.0)
project(test1-vn VERSION 0.1.0 LANGUAGES CXX)

# Comment out if needed; since I am the primary user of this file, this makes 100% sure that I use the correct compiler
#set(CMAKE_CXX_COMPILER "g++")

set(BOOST_ROOT ./boost_1_79_0/)
#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost 1.79.0) 

cmake_policy(SET CMP0076 NEW)



# This apparently may cause issues with non-makefile generators. Comment out if needed
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE TRUE)


#add_executable(test1 main.cpp)

add_executable(test1 testmain.cpp)

target_sources(test1 PUBLIC video-sfml-text.cpp video-sfml-compositor.cpp trees.cpp)

# this needs improvement
target_include_directories(test1 PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(test1 PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)

# Those sweet, sweet designated initialisers force me to use C++20
set_property(TARGET test1 PROPERTY CXX_STANDARD 20)
set_property(TARGET test1 PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET test1 PROPERTY CXX_STANDARD_REQUIRED ON)

if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(test1 PRIVATE -Wall -Wextra -Wno-unused-parameter -pthread -fmodules-ts)
endif()
