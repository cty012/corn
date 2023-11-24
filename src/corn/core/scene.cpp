#include <corn/core/scene.h>

namespace corn {
    Scene::Scene() : game_(nullptr), systems_() {
        static SceneID uniqueID = 0;
        this->id_ = uniqueID++;
        this->room_ = "Scene::" + std::to_string(this->id_);
        EventManager::addRoom(this->room_);
        this->entityManager_ = new EntityManager(*this);
        this->uiManager_ = new UIManager(*this);
    }

    Scene::~Scene() {
        for (corn::System* system : this->systems_) {
            delete system;
        }
        delete this->entityManager_;
        EventManager::removeRoom(this->room_);
    }

    Scene::SceneID Scene::getID() const {
        return this->id_;
    }

    const Game* Scene::getGame() const {
        return this->game_;
    }

    EntityManager& Scene::getEntityManager() const {
        return *this->entityManager_;
    }

    UIManager& Scene::getUIManager() const {
        return *this->uiManager_;
    }

    EventManager& Scene::getEventManager() const {
        return EventManager::instance(this->room_);
    }

    void Scene::update(float millis) {
        for (System* system : this->systems_) {
            if (system->active) {
                system->update(millis);
            }
        }
    }
}
