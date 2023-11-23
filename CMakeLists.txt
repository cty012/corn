cmake_minimum_required(VERSION 3.22)
project(corn)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps")

option(BUILD_EXAMPLES "Build the example programs in the `examples/` folder." ON)
option(BUILD_TESTS "Build the test cases in the `test/` folder." OFF)

if (MSVC)
    add_compile_options(/utf-8 /W4 /WX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif ()

set(CORN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
set(EXAMPLES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/examples")
set(TEST_OUTPUT_DIR "${CMAKE_BINARY_DIR}/tests")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS
        "${CMAKE_SOURCE_DIR}/include/*.h"
        "${CMAKE_SOURCE_DIR}/include/*.cpp"
)
file(GLOB_RECURSE SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(corn SHARED ${HEADERS} ${SOURCES})
set_target_properties(corn PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CORN_OUTPUT_DIR}")
set(CORN_OUTPUT_PATH $<TARGET_FILE:corn>)
target_include_directories(corn PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Link with SFML
#if (WIN32)
#    set(VCPKG_TARGET_ARCHITECTURE x64)
#elseif (APPLE)
#    set(SFML_ROOT "${CMAKE_SOURCE_DIR}/deps/${SFML_PACKAGE_NAME}")
#    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
#    set(SFML_LIBRARY_DIR "${SFML_ROOT}/lib")
#    set(SFML_LIBRARIES
#            ${SFML_LIBRARY_DIR}/libsfml-audio.dylib
#            ${SFML_LIBRARY_DIR}/libsfml-graphics.dylib
#            ${SFML_LIBRARY_DIR}/libsfml-network.dylib
#            ${SFML_LIBRARY_DIR}/libsfml-system.dylib
#            ${SFML_LIBRARY_DIR}/libsfml-window.dylib
#    )
#    target_include_directories(corn PRIVATE ${SFML_INCLUDE_DIR})
#elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
#endif ()

find_package(SFML 2.6 COMPONENTS audio graphics network system window CONFIG REQUIRED)
set(SFML_LIBRARIES
        sfml-audio
        sfml-graphics
        sfml-network
        sfml-system
        sfml-window
)

target_link_libraries(corn PRIVATE ${SFML_LIBRARIES})

# Build examples
if (BUILD_EXAMPLES)
    add_subdirectory("examples")
endif ()

# Build test cases
if (BUILD_TESTS)
    add_subdirectory("test")
endif ()
