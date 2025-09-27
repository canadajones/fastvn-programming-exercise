cmake_minimum_required(VERSION 3.13.0)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0144 NEW)


project(test1-vn VERSION 0.1.0 LANGUAGES CXX)


set(BOOST_ROOT ./extern/boost_1_89_0/)
set(BOOST_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
set(BOOST_USE_STATIC_RUNTIME ON) 

set(BOOST_ENABLE_CMAKE ON)


find_package(Boost 1.89.0) 

add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)
add_subdirectory(extern/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(extern/SDL_ttf EXCLUDE_FROM_ALL)


# This apparently may cause issues with non-makefile generators. Comment out if needed
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# todo: make this a module thing
add_library(metasyntax src/parser/metasyntax/metasyntax.cpp)
add_library(scriptflow src/parser/scriptflow/scriptflow.cpp)
add_library(parser src/parser/translation.cpp)
target_link_libraries(parser metasyntax scriptflow)

add_executable(metasyntax_test tests/metasyntax.test.cpp) 
add_executable(scriptflow_test tests/scriptflow.test.cpp)
add_executable(translation_test tests/translation.test.cpp)
add_executable(gfx_sdl_test tests/gfx.sdl.test.cpp)
add_executable(main src/main/main.cpp)


# this needs improvement
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})


target_link_libraries(metasyntax_test metasyntax)
target_link_libraries(scriptflow_test scriptflow)
target_link_libraries(translation_test parser)
target_link_libraries(gfx_sdl_test SDL3::SDL3)
target_link_libraries(main parser SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)

# Those sweet, sweet designated initialisers force me to use C++20
set_property(TARGET metasyntax_test PROPERTY CXX_STANDARD 20)
set_property(TARGET metasyntax_test PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET metasyntax_test PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET scriptflow_test PROPERTY CXX_STANDARD 20)
set_property(TARGET scriptflow_test PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET scriptflow_test PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET gfx_sdl_test PROPERTY CXX_STANDARD 20)
set_property(TARGET gfx_sdl_test PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET gfx_sdl_test PROPERTY CXX_STANDARD_REQUIRED ON)



if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(metasyntax_test PRIVATE -ftemplate-backtrace-limit=0 -Wfatal-errors -Wall -Wextra -Wno-unused-parameter -pthread)
	target_compile_options(scriptflow_test PRIVATE -ftemplate-backtrace-limit=0 -Wfatal-errors -Wall -Wextra -Wno-unused-parameter -pthread)
	target_compile_options(gfx_sdl_test PRIVATE -ftemplate-backtrace-limit=0 -Wfatal-errors -Wall -Wextra -Wno-unused-parameter -pthread)
endif()
