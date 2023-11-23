#pragma once

#include <corn/util.h>

constexpr size_t WIDTH = 1280;
constexpr size_t HEIGHT = 720;

constexpr size_t BIRD_WIDTH = 50;
constexpr size_t BIRD_HEIGHT = 50;
const corn::Color BIRD_COLOR = corn::Color::rgb(255, 0, 0);

constexpr size_t CEIL_THICKNESS = 40;
const corn::Color CEIL_COLOR = corn::Color::rgb(255, 128, 0);

constexpr float WALL_THICKNESS = 120.0f;
constexpr float WALL_SPEED = 200.0f;
constexpr float WALL_INTERVAL = 300.0f;
const corn::Color WALL_COLOR = corn::Color::rgb(50, 205, 50);

constexpr size_t HOLE_MIN_PADDING = 120;
constexpr size_t HOLE_SIZE = 260;
