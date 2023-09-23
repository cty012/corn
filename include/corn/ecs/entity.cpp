namespace corn {
    template<ComponentType T, typename... Args>
    T* Entity::addComponent(Args&&... args) {
        auto key = std::type_index(typeid(T));
        if (this->components.find(key) != this->components.end()) return nullptr;
        T* component = new T(*this, std::forward<Args>(args)...);
        this->components[key] = component;
        return component;
    }

    template<ComponentType T>
    T* Entity::getComponent() {
        auto key = std::type_index(typeid(T));
        if (this->components.find(key) == this->components.end()) return nullptr;
        return dynamic_cast<T*>(this->components[key]);
    }

    template<ComponentType T>
    bool Entity::removeComponent() {
        auto key = std::type_index(typeid(T));
        if (this->components.find(key) == this->components.end()) return false;
        this->components.erase(key);
        return true;
    }
}
