#pragma once

namespace corn {
    template<WidgetType T, typename... Args>
    T* UIManager::createWidget(const std::string& name, const UIWidget* parent, Args &&... args) {\
        // Verify parent
        Node* parentNode = this->getNodeFromWidget(parent);

        // Create the entity
        static Entity::EntityID entID = 0;
        while (this->nodes.contains(entID)) {
            entID++;
        }
        T* widget = new T(*this, std::forward<Args>(args)...);

        // Create the node
        this->nodes.emplace(widget->id, Node(widget, parentNode));
        parentNode->children.push_back(&this->nodes.at(widget->id));
        parentNode->dirty = true;

        return widget;
    }
}
