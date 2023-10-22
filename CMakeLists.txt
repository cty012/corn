cmake_minimum_required(VERSION 3.22)
project(corn)

option(BUILD_EXAMPLES "Whether or not to build the example programs in the `examples/` folder." ON)
set(SFML_PACKAGE_NAME "SFML-2.6" CACHE STRING "Name of the SFML folder inside `deps/`. Only used on MacOS.")
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug or Release)")

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps")

# Build the DLL for the library
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

file(GLOB_RECURSE HEADERS
        "${CMAKE_SOURCE_DIR}/include/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(corn SHARED ${HEADERS} ${SOURCES})

target_include_directories(corn PRIVATE "include")

# Link with SFML
if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
    find_package(SFML 2.6 REQUIRED COMPONENTS audio graphics network system window)
    set(SFML_LIBRARIES
            sfml-audio
            sfml-graphics
            sfml-network
            sfml-system
            sfml-window
    )
elseif (APPLE)
    set(SFML_ROOT "${CMAKE_SOURCE_DIR}/deps/${SFML_PACKAGE_NAME}")
    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
    set(SFML_LIBRARY_DIR "${SFML_ROOT}/lib")
    set(SFML_LIBRARIES
            ${SFML_LIBRARY_DIR}/libsfml-audio.dylib
            ${SFML_LIBRARY_DIR}/libsfml-graphics.dylib
            ${SFML_LIBRARY_DIR}/libsfml-network.dylib
            ${SFML_LIBRARY_DIR}/libsfml-system.dylib
            ${SFML_LIBRARY_DIR}/libsfml-window.dylib
    )
    target_include_directories(corn PRIVATE ${SFML_INCLUDE_DIR})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(SFML 2.6 REQUIRED COMPONENTS audio graphics network system window)
    set(SFML_LIBRARIES
            sfml-audio
            sfml-graphics
            sfml-network
            sfml-system
            sfml-window
    )
endif ()

target_link_libraries(corn PRIVATE ${SFML_LIBRARIES})

# Build the examples
function(build_example EXAMPLE_NAME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
    file(GLOB EXAMPLE_HEADERS "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.h")
    file(GLOB EXAMPLE_SOURCES "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.cpp")
    if (WIN32)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
#        add_executable(${EXAMPLE_NAME} WIN32 ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
    elseif (APPLE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "@executable_path"
        )
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN"
        )
    endif ()
    target_include_directories(${EXAMPLE_NAME} PRIVATE "include")
    target_link_libraries(${EXAMPLE_NAME} corn)
endfunction()

if (BUILD_EXAMPLES)
    build_example("clangy-bird")
endif ()
