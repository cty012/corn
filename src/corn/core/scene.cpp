#include <corn/core/scene.h>
#include <corn/ecs/entity_manager.h>
#include <corn/ecs/system.h>
#include <corn/event/event_manager.h>
#include <corn/ui/ui_manager.h>

namespace corn {
    Scene::Scene() noexcept : game_(nullptr), systems_() {
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

    Scene::SceneID Scene::getID() const noexcept {
        return this->id_;
    }

    EntityManager& Scene::getEntityManager() const noexcept {
        return *this->entityManager_;
    }

    UIManager& Scene::getUIManager() const noexcept {
        return *this->uiManager_;
    }

    EventManager& Scene::getEventManager() const noexcept {
        return EventManager::instance(this->room_);
    }

    const Game* Scene::getGame() const noexcept {
        return this->game_;
    }

    void Scene::update(float millis) {
        for (System* system : this->systems_) {
            if (system->isActive()) {
                system->update(millis);
            }
        }
    }
}
