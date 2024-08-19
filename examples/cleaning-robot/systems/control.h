#pragma once

#include <corn/core/scene.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/system.h>
#include <corn/event/event_scope.h>

class SControl : public corn::System {
public:
    SControl(corn::Scene& scene, corn::Entity::EntityID robotID);

    void update(float millis) override;

private:
    void updateMovement();

    corn::Entity::EntityID robotID_;
    float speed_ = 2000;
    corn::EventScope eventScope_;
};
