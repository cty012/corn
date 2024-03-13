#pragma once

#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include "entities.h"

/// A system for managing wall creation and deletion
class WallManager : public corn::System {
public:
    explicit WallManager(corn::Scene& scene);
    void update(float millis) override;
};

/// A custom collision resolve system for bird
class BirdCollisionResolve : public corn::System {
public:
    explicit BirdCollisionResolve(corn::Scene& scene);
    ~BirdCollisionResolve() override;

    void resolve(const corn::EventArgsCollision& args);
    void update(float millis) override;

private:
    bool hasCollided;
    corn::EventManager::ListenerID collisionEventID;
};
