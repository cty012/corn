#include <corn/core/scene.h>
#include <corn/event/event_manager.h>

namespace corn {
    Scene::Scene() : systems(std::vector<System*>()), game(nullptr) {
        static SceneID uniqueID = 0;
        this->id = uniqueID++;
        this->room = "Scene::" + std::to_string(this->id);
        EventManager::addRoom(this->room);
        this->entityManager = new EntityManager(*this);
        this->uiManager = new UIManager(*this);
    }

    Scene::~Scene() {
        delete this->entityManager;
        EventManager::removeRoom(this->room);
    }

    Scene::SceneID Scene::getID() const {
        return this->id;
    }

    const Game* Scene::getGame() const {
        return this->game;
    }

    EntityManager& Scene::getEntityManager() const {
        return *this->entityManager;
    }

    UIManager& Scene::getUIManager() const {
        return *this->uiManager;
    }

    EventManager& Scene::getEventManager() const {
        return EventManager::instance(this->room);
    }

    void Scene::update(float millis) {
        // TODO
        for (System* system : this->systems) {
            if (system->active) {
                system->update(millis);
            }
        }
    }
}
