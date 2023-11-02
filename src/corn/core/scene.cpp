#include <corn/core/scene.h>

namespace corn {
    Scene::Scene()
        : entityManager(EntityManager(*this)), uiManager(UIManager(*this)), systems(std::vector<System*>()) {}

    Scene::~Scene() = default;

    void Scene::update(float millis) {
        // TODO
        for (System* system : this->systems) {
            if (system->active) {
                system->update(this->entityManager, millis);
            }
        }
    }
}
