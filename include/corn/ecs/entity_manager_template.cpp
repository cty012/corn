namespace corn {
    template<ComponentType... T>
    std::vector<Entity*> EntityManager::getEntitiesWith(const Entity* parent, bool recurse) const {
        return getEntitiesHelper([](Entity* entity) {
                return (... && entity->getComponent<T>());
            }, false, 0, parent, recurse);
    }

    template<ComponentType... T>
    std::vector<Entity*> EntityManager::getActiveEntitiesWith(const Entity* parent, bool recurse) const {
        return getEntitiesHelper([](Entity* entity) {
                return (... && entity->getComponent<T>());
            }, true, 0, parent, recurse);
    }
}
