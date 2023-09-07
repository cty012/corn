#include <corn/core.h>


namespace corn {
    Scene::Scene()
        : entityManager(EntityManager()), systems(std::vector<System*>()) {
        static unsigned long long uniqueId = 0;
        this->uniqueId = uniqueId++;
    }

    void Scene::fixedUpdate() {
        for (System* system : this->systems) {
            if (system->active) {
                system->update(0.01); // TODO: use timer to get actual time
            }
        }
    }
}
