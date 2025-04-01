#pragma once

#include <corn/core/scene.h>
#include <corn/ecs/entity.h>
#include <corn/geometry/vec.h>

class MainScene : public corn::Scene {
public:
    MainScene();

    void move(const corn::Vec2f& displacement);
    void rotate(corn::Deg angle);
    void scale(float factor);

private:
    float speed_ = 100.0f;
    corn::Deg angularSpeed_ = 10.0f;
    float scaleSpeed_ = 1.1f;

    corn::EventScope eventScope_;
    corn::Entity::EntityID movableBlockID_{};
    corn::Entity::EntityID cameraID_{};
};
