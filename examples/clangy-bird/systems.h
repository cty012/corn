#pragma once

#include <corn/ecs.h>
#include <corn/event.h>
#include "etities.h"

/// A system for managing wall creation and deletion
class WallManager : public corn::System {
public:
    void update(corn::EntityManager& entityManager, float millis) override;
};

/// A custom collision resolve system for bird
class BirdCollisionResolve : public corn::System {
public:
    BirdCollisionResolve();
    ~BirdCollisionResolve() override;

    void resolve(const corn::EventArgsCollision& args);
    void update(corn::EntityManager& entityManager, float millis) override;

private:
    bool hasCollided;
    corn::EventManager::ListenerID collisionEventID;
};
