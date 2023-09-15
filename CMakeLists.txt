cmake_minimum_required(VERSION 3.26)
project(corn)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps")

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.6 REQUIRED COMPONENTS audio graphics network system window)

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(include)

add_executable(corn ${HEADERS} ${SOURCES})
target_link_libraries(corn sfml-audio sfml-graphics sfml-network sfml-system sfml-window)
