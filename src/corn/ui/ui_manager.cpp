#include <corn/ui/ui_manager.h>

namespace corn {
    UIManager::UIManager(const Config* config, EntityManager* entityManager)
        : config(config), entityManager(entityManager) {}

    UIManager::~UIManager() {
        // TODO: delete UI components
    }

    EntityManager* UIManager::getEntityManager() const {
        return entityManager;
    }
}
