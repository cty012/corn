namespace corn {
    template<ElementType T, typename... Args>
    T* UIManager::createElement(Args &&... args) {
        T* element = new T(*this, std::forward<Args>(args)...);
        // TODO: Add the element to the tree
        return element;
    }
}
