#include <algorithm>
#include <ranges>
#include <stack>
#include <corn/core/scene.h>
#include <corn/geometry/vec4.h>
#include <corn/media/image.h>
#include <corn/ui/ui_image.h>
#include <corn/ui/ui_label.h>
#include <corn/ui/ui_manager.h>
#include "../event/event_args_extend.h"
#include "../media/text_render_impl.h"

namespace corn {
    UIManager::Node::Node(UIWidget* widget, UIManager::Node* parent) noexcept
            : widget(widget), parent(parent), dirty(false) {}

    UIManager::UIManager(Scene& scene) noexcept
            : scene_(scene), root_(nullptr, nullptr), focusedWidget_(nullptr), hoveredWidgets_(), hoveredWidgetSet_() {}

    UIManager::~UIManager() {
        // Delete UI widgets
        for (auto& [id, node] : this->nodes_) {
            delete node.widget;
        }
    }

    Scene& UIManager::getScene() const noexcept {
        return this->scene_;
    }

    const Game* UIManager::getGame() const noexcept {
        return this->scene_.getGame();
    }

    UIWidget* UIManager::getWidgetByID(UIWidget::WidgetID id) const noexcept {
        if (!this->nodes_.contains(id)) return nullptr;
        return this->nodes_.at(id).widget;
    }

    UIWidget* UIManager::getWidgetByName(const std::string& name, const UIWidget* parent, bool recurse) const noexcept {
        std::vector<UIWidget*> result = getWidgetsHelper([&name](const UIWidget* widget) {
            return widget->name_ == name;
        }, false, 1, parent, recurse);
        return result.empty() ? nullptr : result[0];
    }

    std::vector<UIWidget*>
    UIManager::getWidgetsByName(const std::string& name, const UIWidget* parent, bool recurse) const noexcept {
        return getWidgetsHelper([&name](const UIWidget* widget) {
            return widget->name_ == name;
        }, false, 0, parent, recurse);
    }

    UIWidget* UIManager::getWidgetThat(
            const std::function<bool(const UIWidget*)>& pred, const UIWidget* parent, bool recurse) const {

        std::vector<UIWidget*> result = this->getWidgetsHelper(pred, false, 1, parent, recurse);
        return result.empty() ? nullptr : result[0];
    }

    std::vector<UIWidget*> UIManager::getWidgetsThat(
            const std::function<bool(const UIWidget*)>& pred, const UIWidget* parent, bool recurse) const {

        return this->getWidgetsHelper(pred, false, 0, parent, recurse);
    }

    std::vector<UIWidget*> UIManager::getAllWidgets(const UIWidget* parent, bool recurse) const noexcept {
        return this->getWidgetsHelper(nullptr, false, 0, parent, recurse);
    }

    std::vector<UIWidget*> UIManager::getAllActiveWidgets(const UIWidget* parent, bool recurse) const noexcept {
        return this->getWidgetsHelper(nullptr, true, 0, parent, recurse);
    }

    void UIManager::clear() noexcept {
        // Reset hovered widgets
        this->hoveredWidgets_.clear();
        this->hoveredWidgetSet_.clear();
        // Delete child nodes
        for (auto& [id, node] : this->nodes_) {
            delete node.widget;
        }
        this->nodes_.clear();
        // Reset root node
        this->root_.children.clear();
        this->root_.dirty = false;
    }

    void UIManager::tidy() noexcept {
        if (this->root_.dirty) {
            this->root_.dirty = false;
            std::stable_sort(this->root_.children.begin(), this->root_.children.end(),
                             [](Node* left, Node* right) {
                                 return left->widget->getZOrder() < right->widget->getZOrder();
                             });
        }

        for (auto& [id, node] : this->nodes_) {
            if (!node.dirty) continue;
            node.dirty = false;
            std::stable_sort(node.children.begin(), node.children.end(),
                             [](Node* left, Node* right) {
                                 return left->widget->getZOrder() < right->widget->getZOrder();
                             });
        }
    }

    void UIManager::calcGeometry(Vec2 windowSize) {
        std::vector<UIWidget*> widgets = this->getAllActiveWidgets();
        struct Property {
            UIGeometry geometry;
            float x, y, nw, nh, w, h;
        };
        std::unordered_map<const UIWidget*, Property> widgetProps;
        widgetProps[nullptr] = {
                UIGeometry::INDEPENDENT,
                0.0f, 0.0f, windowSize.x, windowSize.y, windowSize.x, windowSize.y,
        };

        // First pass (natural size)
        for (const UIWidget* widget : std::ranges::reverse_view(std::views::all(widgets))) {
            UIGeometry geometry = widget->getActualGeometry();

            // Calculate natural size
            float nw = 0.0f, nh = 0.0f;
            switch (widget->getType()) {
                case UIType::PANEL: {
                    // Avoid the recursion overhead in getNaturalGeometry
                    std::vector<UIWidget*> independentChildren = this->getWidgetsThat(
                            [&widgetProps](const UIWidget* widget) {
                                return widget->isActive() && widgetProps.at(widget).geometry == UIGeometry::INDEPENDENT;
                            },
                            widget, false);
                    // Find max of children size
                    for (UIWidget* child : independentChildren) {
                        nw = std::max(nw, widgetProps[child].w + widgetProps[child].x);
                        nh = std::max(nh, widgetProps[child].h + widgetProps[child].y);
                    }
                    break;
                }
                default: {
                    Vec2 naturalSize = widget->getNaturalSize();
                    nw = naturalSize.x;
                    nh = naturalSize.y;
                    break;
                }
            }

            switch (geometry) {
                case UIGeometry::INDEPENDENT: {
                    float percNW = nw * 0.01f;
                    float percNH = nh * 0.01f;
                    float x = widget->getX().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                    float y = widget->getY().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                    float w = widget->getW().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                    float h = widget->getH().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                    widgetProps[widget] = { geometry, x, y, nw, nh, w, h };
                    break;
                }
                case UIGeometry::DEPENDENT:
                case UIGeometry::DEFAULT: {
                    widgetProps[widget] = { geometry, 0.0f, 0.0f, nw, nh, 0.0f, 0.0f };
                    break;
                }
            }
        }

        // Second pass (location and size)
        for (const UIWidget* widget : widgets) {
            Property& props = widgetProps[widget];
            Property& parentProps = widgetProps[widget->getParent()];
            switch (props.geometry) {
                case UIGeometry::INDEPENDENT: {
                    props.x += parentProps.x;
                    props.y += parentProps.y;
                    break;
                }
                case UIGeometry::DEPENDENT:
                case UIGeometry::DEFAULT: {
                    float percW = parentProps.w * 0.01f;
                    float percH = parentProps.h * 0.01f;
                    float percNW = props.nw * 0.01f;
                    float percNH = props.nh * 0.01f;
                    props.x = widget->getX().calc(1.0f, percW, percH, percNW, percNH) + parentProps.x;
                    props.y = widget->getY().calc(1.0f, percW, percH, percNW, percNH) + parentProps.y;
                    props.w = widget->getW().calc(1.0f, percW, percH, percNW, percNH);
                    props.h = widget->getH().calc(1.0f, percW, percH, percNW, percNH);
                    break;
                }
            }
        }

        // Final pass (assign to node)
        for (const UIWidget* widget : widgets) {
            Property& props = widgetProps[widget];
            Node& node = this->nodes_.at(widget->getID());
            node.location = { props.x, props.y };
            node.size = { props.w, props.h };
        }
    }

    Vec4 UIManager::getCachedGeometry(const UIWidget* widget) const noexcept {
        const Node* node = this->getNodeFromWidget(widget);
        return { node->location.x, node->location.y, node->size.x, node->size.y };
    }

    UIWidget* UIManager::getFocusedWidget() const noexcept {
        return this->focusedWidget_;
    }

    void UIManager::setFocusedWidget(UIWidget* widget) noexcept {
        if (widget && &widget->getUIManager() != this) return;
        if (widget != this->focusedWidget_) {
            UIWidget* oldFocus = this->focusedWidget_;
            this->focusedWidget_ = widget;
            if (oldFocus) {
                oldFocus->getEventManager().emit(EventArgsUIOnUnfocus(oldFocus));
            }
            if (widget) {
                widget->getEventManager().emit(EventArgsUIOnFocus(widget));
            }
        }
    }

    bool UIManager::onKeyboard(const EventArgsKeyboard& args) noexcept {
        bool found = false;
        for (auto& [widgetID, node] : this->nodes_) {
            UIWidget* widget = node.widget;
            if (widget && widget->isActiveInWorld() && widget->isKeyboardInteractable()) {
                widget->getEventManager().emit(EventArgsUIKeyboard(args));
                found = true;
            }
        }
        return found;
    }

    bool UIManager::onClick(const EventArgsMouseButton& args) noexcept {
        UIWidget* widget = this->getTargetWidget(args.mousePos);

        // Bubble up
        UIWidget* newFocus = nullptr;
        for (UIWidget* current = widget; current; current = current->getParent()) {
            if (current->isMouseInteractable() && this->widgetContains(current, args.mousePos)) {
                current->getEventManager().emit(EventArgsUIOnClick(args, widget));
                if (!newFocus) {
                    newFocus = current;
                }
            }
        }

        // Change focus
        this->setFocusedWidget(newFocus);

        return widget != nullptr;
    }

    bool UIManager::onHover(const EventArgsMouseMove& args) noexcept {
        UIWidget* widget = this->getTargetWidget(args.mousePos);

        // Bubble up
        std::vector<UIWidget*> newHoveredWidgets;
        std::unordered_set<UIWidget*> newHoveredWidgetSet;
        for (UIWidget* current = widget; current; current = current->getParent()) {
            if (current->isMouseInteractable() && this->widgetContains(current, args.mousePos)) {
                newHoveredWidgets.push_back(current);
                newHoveredWidgetSet.insert(current);
            }
        }

        // On exit
        for (UIWidget* current : this->hoveredWidgets_) {
            if (!newHoveredWidgetSet.contains(current)) {
                current->getEventManager().emit(EventArgsUIOnExit(args, widget));
            }
        }

        // On enter
        for (UIWidget* current : newHoveredWidgets) {
            if (!this->hoveredWidgetSet_.contains(current)) {
                current->getEventManager().emit(EventArgsUIOnEnter(args, widget));
            }
        }

        // On hover
        for (UIWidget* current : newHoveredWidgets) {
            current->getEventManager().emit(EventArgsUIOnHover(args, widget));
        }

        // Update hovered widgets
        this->hoveredWidgets_ = std::move(newHoveredWidgets);
        this->hoveredWidgetSet_ = std::move(newHoveredWidgetSet);

        return !this->hoveredWidgets_.empty();
    }

    bool UIManager::onScroll(const EventArgsMouseScroll& args) noexcept {
        UIWidget* widget = this->getTargetWidget(args.mousePos);

        // Bubble up
        for (UIWidget* current = widget; current; current = current->getParent()) {
            if (current->isMouseInteractable() && this->widgetContains(current, args.mousePos)) {
                current->getEventManager().emit(EventArgsUIOnScroll(args, widget));
            }
        }

        return widget;
    }

    bool UIManager::onTextEntered(const EventArgsTextEntered& args) noexcept {
        if (!this->focusedWidget_) return false;
        this->focusedWidget_->getEventManager().emit(args);
        return true;
    }

    bool UIManager::widgetContains(const UIWidget* widget, Vec2 pos) const noexcept {
        auto [x, y, w, h] = this->getCachedGeometry(widget);  // NOLINT
        return x < pos.x && y < pos.y && x + w > pos.x && y + h > pos.y;
    }

    UIWidget* UIManager::getTargetWidget(Vec2 pos) noexcept {
        this->tidy();
        std::vector<UIWidget*> widgets = this->getAllActiveWidgets();
        for (UIWidget* widget : std::ranges::reverse_view(std::views::all(widgets))) {
            if (widget->isMouseInteractable() && this->widgetContains(widget, pos)) return widget;
        }
        return nullptr;
    }

    void UIManager::destroyNode(Node* node) noexcept {
        if (node == nullptr) return;
        // Destroy all children
        for (Node* child : node->children) {
            UIManager::destroyNode(child);
        }
        // Remove from hovered
        std::erase(this->hoveredWidgets_, node->widget);
        this->hoveredWidgetSet_.erase(node->widget);
        // Destroy self
        UIWidget::WidgetID widgetID = node->widget->getID();
        delete node->widget;
        this->nodes_.erase(widgetID);
    }

    void UIManager::destroyWidget(UIWidget& widget) noexcept {
        Node* node = &this->nodes_.at(widget.getID());
        Node* parent = node->parent;
        // Removes relation (parent --> node)
        std::erase(parent->children, node);
        // Destroy node itself
        this->destroyNode(node);
    }

    const UIManager::Node* UIManager::getNodeFromWidget(const UIWidget* widget) const {
        if (!widget) {
            return &this->root_;
        } else if (&widget->getUIManager() == this) {
            return &this->nodes_.at(widget->getID());
        } else {
            throw std::invalid_argument("Parent widget must be created by the same UI Manager.");
        }
    }

    UIManager::Node* UIManager::getNodeFromWidget(const UIWidget* widget) {
        if (!widget) {
            return &this->root_;
        } else if (&widget->getUIManager() == this) {
            return &this->nodes_.at(widget->getID());
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
            if (onlyActive && (next != &root_) && !next->widget->isActive()) continue;

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
