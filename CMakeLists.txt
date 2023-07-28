cmake_minimum_required(VERSION 3.20.3)

project(arg_parser)

# Set the version of your project
set(ARG_PARSER_VERSION "0.0.1")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_PROJECT_NAME}/include)

add_subdirectory(${CMAKE_PROJECT_NAME})
add_subdirectory(${CMAKE_PROJECT_NAME}_tests)

# Dependencies
add_subdirectory(deps/googletest)
