#pragma once

#include <corn/ecs.h>

/// Component for identifying walls
struct Wall : public corn::Component {
    explicit Wall(corn::Entity& entity);
};
