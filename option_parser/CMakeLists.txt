set(BINARY ${CMAKE_PROJECT_NAME})

# Collect all header files
file(GLOB_RECURSE HEADERS LIST_DIRECTORIES true *.h *.hpp)

# Collect all source files
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.c *.cpp)

# Create the library
add_library(${BINARY}_lib STATIC ${SOURCES} ${HEADERS})

# Include the 'include' directory as a public directory
target_include_directories(${BINARY}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)