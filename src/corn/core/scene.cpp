#include <corn/core/scene.h>

namespace corn {
    Scene::Scene() : entityManager(EntityManager()), systems(std::vector<System*>()) {}

    void Scene::update(float millis) {
        // TODO
        for (System* system : this->systems) {
            if (system->active) {
                system->update(this->entityManager, millis);
            }
        }
    }
}
