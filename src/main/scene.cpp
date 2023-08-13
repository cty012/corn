#include <main/scene.h>


namespace corn {
    Scene::Scene() : entityManager(EntityManager()),  {
        static uniqueId = 0;
        this->uniqueId = uniqueId++;
    }

    void Scene::fixedUpdate() {
        for (System* system : this->systems) {
            if (system->active) {
                system->fixedUpdate();
            }
        }
    }
}
