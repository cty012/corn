cmake_minimum_required(VERSION 3.22)

# CMake cache variables
option(BUILD_EXAMPLES "Build the example programs in the `examples/` folder." ON)
option(BUILD_TESTS "Build the test cases in the `test/` folder." OFF)
option(USE_NATIVE_TEXT_RENDERING "Enable native text rendering" ON)

if (BUILD_TESTS)
    enable_testing()
    set(VCPKG_MANIFEST_FEATURES "${VCPKG_MANIFEST_FEATURES};tests")
endif ()

# Project settings
project(corn VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(DEPS_PATH "${CMAKE_SOURCE_DIR}/deps")
list(APPEND CMAKE_PREFIX_PATH "${DEPS_PATH}")

if (APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../lib;@loader_path/../lib/external;@loader_path/../Frameworks")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib;$ORIGIN/../lib/external;$ORIGIN/../Frameworks")
endif ()

# Output directories
set(CORN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
set(EXAMPLES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/examples")
set(TEST_OUTPUT_DIR "${CMAKE_BINARY_DIR}/tests")

# OS and Platform
if (WIN32)
    set(OS "win")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM "win64")
    else ()
        set(PLATFORM "win32")
    endif ()
elseif (APPLE)
    set(OS "macos")
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(PLATFORM "macos-arm64")
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(PLATFORM "macos-x64")
    else ()
        set(PLATFORM "macos")
    endif ()
elseif (UNIX)
    set(OS "linux")
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        set(PLATFORM "linux-x64")
    else ()
        set(PLATFORM "linux")
    endif ()
else ()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_SYSTEM_NAME}")
    set(OS "unknown")
    set(PLATFORM "unknown")
endif ()

# Compiler flags
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#endif()
if (MSVC)
    add_compile_options(/utf-8 /W4 /WX /Zc:__cplusplus /Zc:preprocessor)
    add_compile_options("$<$<CONFIG:Release>:/Ox>")
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
)
file(GLOB_RECURSE PRIVATE_HEADERS
        "${CMAKE_SOURCE_DIR}/src/corn/core/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/ecs/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/event/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/geometry/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/media/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/render/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/ui/*.h"
        "${CMAKE_SOURCE_DIR}/src/corn/util/*.h"
)
file(GLOB_RECURSE SOURCES
        "${CMAKE_SOURCE_DIR}/src/corn/core/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/ecs/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/event/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/geometry/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/media/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/render/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/ui/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/util/*.cpp"
)
file(GLOB_RECURSE NATIVE_HEADERS
        "${CMAKE_SOURCE_DIR}/src/corn/native/include/${OS}/*.h"
)
file(GLOB_RECURSE NATIVE_SOURCES
        "${CMAKE_SOURCE_DIR}/src/corn/native/${OS}/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/corn/native/${OS}/*.mm"
)

add_library(corn SHARED
        ${HEADERS} ${PRIVATE_HEADERS} ${SOURCES}
        ${NATIVE_HEADERS} ${NATIVE_SOURCES}
)
set_target_properties(corn PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CORN_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CORN_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CORN_OUTPUT_DIR}"
        DEBUG_POSTFIX -d
)
set(CORN_OUTPUT_PATH $<TARGET_FILE:corn>)
target_include_directories(corn PRIVATE
        "${CMAKE_SOURCE_DIR}/include/"
        "${CMAKE_SOURCE_DIR}/src/corn/native/include/"
)

# Link with native libraries
if (USE_NATIVE_TEXT_RENDERING)
    target_compile_definitions(corn PRIVATE USE_NATIVE_TEXT_RENDERING)
    if (MSVC)
    elseif (APPLE)
        target_link_libraries(corn PRIVATE
                "-framework CoreGraphics"
                "-framework ImageIO"
                "-framework UniformTypeIdentifiers"
                "-framework Foundation"
        )
    elseif (UNIX)
    endif ()
endif ()

# Link with SFML
#find_package(SFML 2.6 COMPONENTS graphics window audio network system CONFIG REQUIRED)
#target_link_libraries(corn PRIVATE sfml-graphics sfml-window sfml-audio sfml-network sfml-system)

# Link with bgfx
find_package(bgfx CONFIG REQUIRED)
target_link_libraries(corn PRIVATE bgfx::bgfx)

# Link with GLFW
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(corn PRIVATE glfw)
if (MSVC)
    target_compile_definitions(corn PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
elseif (APPLE)
    target_compile_definitions(corn PRIVATE GLFW_EXPOSE_NATIVE_COCOA)
elseif (UNIX)
    target_compile_definitions(corn PRIVATE GLFW_EXPOSE_NATIVE_X11)
endif ()

# Link with ICU
find_package(ICU REQUIRED COMPONENTS uc i18n data)
target_link_libraries(corn PRIVATE ICU::uc ICU::i18n ICU::data)

# Link with Eigen3
find_package(Eigen3 REQUIRED NO_MODULE)
target_link_libraries(corn PRIVATE Eigen3::Eigen)

# Link with NanoSVG
find_package(NanoSVG CONFIG REQUIRED)
target_link_libraries(corn PRIVATE NanoSVG::nanosvg NanoSVG::nanosvgrast)

# Link with Boost::Geometry
find_package(Boost CONFIG REQUIRED geometry)
target_link_libraries(corn PRIVATE Boost::geometry)

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

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION ".")
