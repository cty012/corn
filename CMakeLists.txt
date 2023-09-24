cmake_minimum_required(VERSION 3.26)
project(corn)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps")

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
    set(SFML_ROOT "${CMAKE_SOURCE_DIR}/deps/SFML-2.6.0")
    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
    set(SFML_LIBRARY_DIR "${SFML_ROOT}/lib")
    set(SFML_LIBRARIES
            ${SFML_LIBRARY_DIR}/libsfml-audio.dylib
            ${SFML_LIBRARY_DIR}/libsfml-graphics.dylib
            ${SFML_LIBRARY_DIR}/libsfml-network.dylib
            ${SFML_LIBRARY_DIR}/libsfml-system.dylib
            ${SFML_LIBRARY_DIR}/libsfml-window.dylib
    )
    include_directories(${SFML_INCLUDE_DIR})
endif ()

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

include_directories(include)

# Build the DLL for the library
add_library(corn SHARED ${HEADERS} ${SOURCES})
target_link_libraries(corn PRIVATE ${SFML_LIBRARIES})

# Build the examples
function(build_example EXAMPLE_NAME)
    file(GLOB EXAMPLE_HEADERS "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.h")
    file(GLOB EXAMPLE_SOURCES "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.cpp")
    if (WIN32)
        add_executable(${EXAMPLE_NAME} WIN32 ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
    elseif (APPLE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "@executable_path"
        )
    endif ()
    target_link_libraries(${EXAMPLE_NAME} corn)
endfunction()

build_example(clangy-bird)
