cmake_minimum_required(VERSION 3.13.0)
project(test1-vn VERSION 0.1.0 LANGUAGES CXX)


set(BOOST_ROOT ./boost_1_81_0/)
#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost 1.81.0) 

cmake_policy(SET CMP0076 NEW)



# This apparently may cause issues with non-makefile generators. Comment out if needed
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_VERBOSE_MAKEFILE TRUE)




add_executable(versiontest src/versioning/versioning.cpp)



# this needs improvement
target_include_directories(versiontest PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(versiontest PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)

# Those sweet, sweet designated initialisers force me to use C++20
set_property(TARGET versiontest PROPERTY CXX_STANDARD 20)
set_property(TARGET versiontest PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET versiontest PROPERTY CXX_STANDARD_REQUIRED ON)

if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(versiontest PRIVATE -Wall -Wextra -Wno-unused-parameter -pthread -fmodules-ts)
endif()
