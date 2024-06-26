cmake_minimum_required(VERSION 3.22)

# CMake cache variables
option(BUILD_EXAMPLES "Build the example programs in the `examples/` folder." ON)
option(BUILD_TESTS "Build the test cases in the `test/` folder." OFF)

if (BUILD_TESTS)
    enable_testing()
    set(VCPKG_MANIFEST_FEATURES "${VCPKG_MANIFEST_FEATURES};tests")
endif ()

# Project settings
project(corn VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(DEPS_PATH "${CMAKE_SOURCE_DIR}/deps")
list(APPEND CMAKE_PREFIX_PATH "${DEPS_PATH}")

# Output directories
set(CORN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
set(EXAMPLES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/examples")
set(TEST_OUTPUT_DIR "${CMAKE_BINARY_DIR}/tests")

# Platform
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM "win64")
    else()
        set(PLATFORM "win32")
    endif()
elseif(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(PLATFORM "macos-arm64")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(PLATFORM "macos-x64")
    else()
        set(PLATFORM "macos")
    endif()
elseif(UNIX)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        set(PLATFORM "linux-x64")
    else()
        set(PLATFORM "linux")
    endif()
else()
    set(PLATFORM "unknown")
endif()

# Compiler flags
if (MSVC)
    add_compile_options(/utf-8 /W4 /WX)
    add_compile_options("$<$<CONFIG:Release>:/O2>")
    add_compile_options("$<$<CONFIG:Debug>:/Zi>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else ()
    add_compile_options(-Wall -Wextra -Werror)
    add_compile_options("$<$<CONFIG:Release>:-O3>")
    add_compile_options("$<$<CONFIG:Debug>:-g>")
endif ()

# For static analysis and linting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Corn target
file(GLOB_RECURSE HEADERS
        "${CMAKE_SOURCE_DIR}/include/*.h"
        "${CMAKE_SOURCE_DIR}/include/*.cpp"
)
file(GLOB_RECURSE SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(corn SHARED ${HEADERS} ${SOURCES})
set_target_properties(corn PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CORN_OUTPUT_DIR}"
        DEBUG_POSTFIX -d
)
set(CORN_OUTPUT_PATH $<TARGET_FILE:corn>)
target_include_directories(corn PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Link with SFML
find_package(SFML 2.6 COMPONENTS audio graphics network system window CONFIG REQUIRED)
target_link_libraries(corn PRIVATE sfml-audio sfml-graphics sfml-network sfml-system sfml-window)

# Link with ICU
find_package(ICU REQUIRED COMPONENTS uc i18n data)
target_link_libraries(corn PRIVATE ICU::uc ICU::i18n ICU::data)

# Link with earcut
find_path(EARCUT_HPP_INCLUDE_DIRS "mapbox/earcut.hpp")
target_include_directories(corn PRIVATE "${EARCUT_HPP_INCLUDE_DIRS}")

# Build examples
if (BUILD_EXAMPLES)
    add_subdirectory("examples")
endif ()

# Build test cases
if (BUILD_TESTS)
    add_subdirectory("test")
endif ()

# Install
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist/corn-${PROJECT_VERSION}-${PLATFORM}")
install(TARGETS corn
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "bin"
        ARCHIVE DESTINATION "lib"
)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/corn" DESTINATION "include")

configure_file(corn-config.cmake.in corn-config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/corn-config.cmake" DESTINATION "cmake")
