#include <algorithm>
#include <stack>
#include <corn/ui/ui_manager.h>
#include <corn/util/exceptions.h>

namespace corn {
    UIManager::Node::Node(UIWidget* widget, UIManager::Node* parent): widget(widget), parent(parent), dirty(false) {}

    UIManager::UIManager(Scene& scene): scene(scene), root(nullptr, nullptr) {}

    UIManager::~UIManager() {
        // Delete UI widgets
        for (auto& [id, node] : this->nodes) {
            delete node.widget;
        }
    }

    std::vector<UIWidget*> UIManager::getAllWidgets(const UIWidget* parent, bool recurse) const {
        return this->getWidgetsHelper(nullptr, false, 0, parent, recurse);
    }

    std::vector<UIWidget*> UIManager::getAllActiveWidgets(const UIWidget* parent, bool recurse) const {
        return this->getWidgetsHelper(nullptr, true, 0, parent, recurse);
    }

    void UIManager::tidy() {
        // TODO: implement this and UI z-order
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
            throw std::invalid_argument("Parent widget must be created by the same UI Manager.");
        }
    }

    UIManager::Node* UIManager::getNodeFromWidget(const UIWidget* widget) {
        if (!widget) {
            return &this->root;
        } else if (&widget->uiManager == this) {
            return &this->nodes.at(widget->id);
        } else {
            throw std::invalid_argument("Parent widget must be created by the same UI Manager.");
        }
    }

    std::vector<UIWidget*> UIManager::getWidgetsHelper(
            const std::function<bool(UIWidget*)>& pred, bool onlyActive, size_t limit,
            const UIWidget* parent, bool recurse) const {

        auto widgets = std::vector<UIWidget*>();
        auto nodeStack = std::stack<const Node*>();
        const Node* parentNode = this->getNodeFromWidget(parent);
        std::for_each(parentNode->children.rbegin(), parentNode->children.rend(), [&nodeStack](Node *child) {
            nodeStack.push(child);
        });
        while (!nodeStack.empty()) {
            // Retrieve the next node
            const Node* next = nodeStack.top();
            nodeStack.pop();

            // Skip if not active
            if (onlyActive && (next != &root) && !next->widget->active) continue;

            // Add widget pointer to vector if current widget satisfy conditions
            if (!pred || pred(next->widget)) {
                widgets.push_back(next->widget);
                if ((--limit) == 0) break;
            }

            // Add children to stack
            if (recurse) {
                std::for_each(next->children.rbegin(), next->children.rend(), [&nodeStack](Node *child) {
                    nodeStack.push(child);
                });
            }
        }
        return widgets;
    }
}
