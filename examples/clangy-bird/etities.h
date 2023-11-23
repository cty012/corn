#pragma once

#include <corn/ecs.h>

corn::Entity* createCamera(corn::EntityManager& entityManager);
corn::Entity* createBird(corn::EntityManager& entityManager);
corn::Entity* createWall(corn::EntityManager& entityManager, float x);
void createCeilAndFloor(corn::EntityManager& entityManager);
