cmake_minimum_required(VERSION 3.13.0)
project(test1-vn VERSION 0.1.0 LANGUAGES CXX)


set(BOOST_ROOT ./extern/boost_1_81_0/)
set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 

set(BOOST_ENABLE_CMAKE ON)


find_package(Boost 1.81.0) 

cmake_policy(SET CMP0076 NEW)



# This apparently may cause issues with non-makefile generators. Comment out if needed
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_VERBOSE_MAKEFILE TRUE)




add_executable(metasyntax_test tests/metasyntax.test.cpp src/parser/metasyntax/metasyntax.cpp)
add_executable(scriptflow_test tests/scriptflow.test.cpp src/parser/scriptflow/scriptflow.cpp)


# this needs improvement
target_include_directories(metasyntax_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})
target_include_directories(scriptflow_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})



target_link_libraries(metasyntax_test PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)

# Those sweet, sweet designated initialisers force me to use C++20
set_property(TARGET metasyntax_test PROPERTY CXX_STANDARD 20)
set_property(TARGET metasyntax_test PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET metasyntax_test PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET scriptflow_test PROPERTY CXX_STANDARD 20)
set_property(TARGET scriptflow_test PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET scriptflow_test PROPERTY CXX_STANDARD_REQUIRED ON)


if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(metasyntax_test PRIVATE -ftemplate-backtrace-limit=0 -Wfatal-errors -Wall -Wextra -Wno-unused-parameter -pthread -fmodules-ts)
	target_compile_options(scriptflow_test PRIVATE -ftemplate-backtrace-limit=0 -Wfatal-errors -Wall -Wextra -Wno-unused-parameter -pthread -fmodules-ts)
endif()
