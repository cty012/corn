cmake_minimum_required(VERSION 3.26)
project(corn)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(include)

add_executable(corn ${HEADERS} ${SOURCES})
#add_executable(corn
#        src/main.cpp
#        include/corn/core.h
#        include/corn/core/display.h
#        include/corn/core/game.h
#        include/corn/core/scene.h
#        include/corn/ecs.h
#        include/corn/ecs/component.h
#        include/corn/ecs/entity.h
#        include/corn/ecs/entity_manager.h
#        include/corn/ecs/system.h
#        include/corn/event.h
#        include/corn/event/event_args.h
#        include/corn/event/event_manager.h
#        include/corn/event/input.h
#        include/corn/util.h
#        include/corn/util/color.h
#        include/corn/util/config.h
#        include/corn/util/exceptions.h
#        include/corn/util/geometry.h
#        src/corn/core/game.cpp
#        src/corn/core/scene.cpp
#        src/corn/ecs/component.cpp
#        src/corn/ecs/entity.cpp
#        src/corn/ecs/entity_manager.cpp
#        src/corn/ecs/system.cpp
#        src/corn/event/event_args.cpp
#        src/corn/event/event_manager.cpp
#        src/corn/util/vec2.cpp
#        src/corn/util/vec3.cpp)
