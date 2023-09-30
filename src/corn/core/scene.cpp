#include <corn/core/scene.h>

namespace corn {
    Scene::Scene()
        : entityManager(EntityManager()), uiManager(UIManager(&entityManager)), systems(std::vector<System*>()) {}

    void Scene::update(double millis) {
        for (System* system : this->systems) {
            if (system->active) {
                system->update(this->entityManager, millis);
            }
        }
    }
}
