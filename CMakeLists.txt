cmake_minimum_required(VERSION 3.26)
project(corn)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(include/corn)
include_directories(include/corn/core)
include_directories(include/corn/ecs)
include_directories(include/corn/util)

add_executable(corn
        include/corn/core/display.h
        include/corn/core/game.h
        include/corn/core/scene.h
        include/corn/ecs/component.h
        include/corn/ecs/entity.h
        include/corn/ecs/system.h
        include/corn/util/color.h
        include/corn/util/config.h
        include/corn/util/geometry.h
        include/corn/core.h
        include/corn/ecs.h
        include/corn/util.h
        src/corn/core/scene.cpp
        src/corn/ecs/entity.cpp
        src/corn/ecs/srender.cpp
        src/corn/util/vec2.cpp
        src/corn/util/vec3.cpp
        src/main.cpp
        src/corn/core/game.cpp)
