#include <corn/core/scene.h>

namespace corn {
    Scene::Scene() : entityManager(EntityManager()), systems(std::vector<System*>()) {
        // TODO
    }

    Scene::~Scene() {
        // TODO
    }

    void Scene::update() {
        // TODO
        for (System* system : this->systems) {
            if (system->active) {
                system->update();
            }
        }
    }
}
