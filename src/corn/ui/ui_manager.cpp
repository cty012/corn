#include <corn/ui/ui_manager.h>

namespace corn {
    UIManager::UIManager(EntityManager* entityManager)
        : entityManager(entityManager) {}

    UIManager::~UIManager() {
        // TODO: delete UI components
    }

    EntityManager* UIManager::getEntityManager() const {
        return entityManager;
    }
}
