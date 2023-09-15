cmake_minimum_required(VERSION 3.26)
project(corn)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps")

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.6 REQUIRED COMPONENTS audio graphics network system window)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

include_directories(include)

# Build the DLL for the library
add_library(corn SHARED ${HEADERS} ${SOURCES})
target_link_libraries(corn sfml-audio sfml-graphics sfml-network sfml-system sfml-window)

# Build the examples
function(build_example EXAMPLE_NAME)
    file(GLOB EXAMPLE_HEADERS "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.h")
    file(GLOB EXAMPLE_SOURCES "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.cpp")
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
    target_link_libraries(${EXAMPLE_NAME} corn)
endfunction()

build_example(platformer)
