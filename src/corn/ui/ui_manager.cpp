#include <corn/ui/ui_manager.h>

namespace corn {
    UIManager::Node::Node(UIWidget* widget, UIManager::Node* parent): widget(widget), parent(parent), dirty(false) {}

    UIManager::UIManager(Scene& scene): scene(scene), root(nullptr, nullptr) {}

    UIManager::~UIManager() {
        // Delete UI widgets
        for (auto& [id, node] : this->nodes) {
            delete node.widget;
        }
    }

    void UIManager::destroyNode(Node* node) {
        if (node == nullptr) return;
        // Destroy all children
        for (Node* child : node->children) {
            UIManager::destroyNode(child);
        }
        // Destroy self
        UIWidget::WidgetID widgetID = node->widget->id;
        delete node->widget;
        this->nodes.erase(widgetID);
    }

    void UIManager::destroyWidget(UIWidget& widget) {
        Node* node = &this->nodes.at(widget.id);
        Node* parent = node->parent;
        this->destroyNode(node);
        // Removes relation (parent --> node)
        parent->children.erase(
                std::remove(parent->children.begin(), parent->children.end(), node),
                parent->children.end());
    }

    const UIManager::Node* UIManager::getNodeFromWidget(const UIWidget* widget) const {
        if (!widget) {
            return &this->root;
        } else if (&widget->uiManager == this) {
            return &this->nodes.at(widget->id);
        } else {
            throw std::invalid_argument("Parent Entity must be created by the same Entity Manager.");
        }
    }

    UIManager::Node* UIManager::getNodeFromWidget(const UIWidget* widget) {
        return const_cast<UIManager::Node*>(static_cast<const UIManager*>(this)->getNodeFromWidget(widget));
    }
}
