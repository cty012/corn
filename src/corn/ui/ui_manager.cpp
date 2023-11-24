#include <algorithm>
#include <ranges>
#include <stack>
#include <corn/core/scene.h>
#include <corn/media/rich_text.h>
#include <corn/ui/ui_label.h>
#include "../media/rich_text_impl.h"
#include <corn/ui/ui_manager.h>
#include <corn/util/exceptions.h>

namespace corn {
    UIManager::Node::Node(UIWidget* widget, UIManager::Node* parent): widget(widget), parent(parent), dirty(false) {}

    UIManager::UIManager(Scene& scene): scene(scene), root(nullptr, nullptr), hoveredWidgets(), hoveredWidgetSet() {
        this->mousebtnEventID = this->scene.getEventManager().addListener(
                "corn::input::mousebtn", [this](const EventArgs& args) {
                    this->onClick(dynamic_cast<const EventArgsMouseButton&>(args));
                });
        this->mousemvEventID = this->scene.getEventManager().addListener(
                "corn::input::mousemv", [this](const EventArgs& args) {
                    this->onHover(dynamic_cast<const EventArgsMouseMove&>(args));
                });
    }

    UIManager::~UIManager() {
        this->scene.getEventManager().removeListener(this->mousebtnEventID);
        this->scene.getEventManager().removeListener(this->mousemvEventID);
        // Delete UI widgets
        for (auto& [id, node] : this->nodes) {
            delete node.widget;
        }
    }

    Scene& UIManager::getScene() const {
        return this->scene;
    }

    const Game* UIManager::getGame() const {
        return this->scene.getGame();
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

    std::vector<UIWidget*> UIManager::getAllWidgets(const UIWidget* parent, bool recurse) const {
        return this->getWidgetsHelper(nullptr, false, 0, parent, recurse);
    }

    std::vector<UIWidget*> UIManager::getAllActiveWidgets(const UIWidget* parent, bool recurse) const {
        return this->getWidgetsHelper(nullptr, true, 0, parent, recurse);
    }

    void UIManager::tidy() {
        // TODO: implement this and UI z-order
    }

    void UIManager::calcGeometry(Vec2 rootSize) {
        std::vector<UIWidget*> widgets = this->getAllActiveWidgets();
        struct Property {
            UIGeometry geometry;
            float x, y, nw, nh, w, h;
        };
        std::unordered_map<const UIWidget*, Property> widgetProps;
        widgetProps[nullptr] = {
                UIGeometry::INDEPENDENT,
                0.0f, 0.0f, rootSize.x, rootSize.y, rootSize.x, rootSize.y,
        };

        // First pass (natural size)
        for (const UIWidget* widget : std::ranges::reverse_view(std::views::all(widgets))) {
            UIGeometry geometry = widget->getGeometry();

            // Calculate natural size
            float nw = 0.0f, nh = 0.0f;
            switch (widget->type) {
                case UIType::PANEL: {
                    std::vector<UIWidget*> independentChildren = this->getWidgetsThat(
                            [&widgetProps](const UIWidget* widget) {
                                return widget->active && widgetProps.at(widget).geometry == UIGeometry::INDEPENDENT;
                            },
                            widget, false);
                    // TODO: find max of children size
                    for (UIWidget* child : independentChildren) {
                        nw = std::max(nw, widgetProps[child].nw + widgetProps[child].x);
                        nh = std::max(nh, widgetProps[child].nh + widgetProps[child].y);
                    }
                    break;
                }
                case UIType::LABEL: {
                    auto nsize = dynamic_cast<const UILabel*>(widget)->getNaturalSize();
                    nw = nsize.x;
                    nh = nsize.y;
                    break;
                }
                case UIType::IMAGE:
                    // TODO
                    break;
                case UIType::INPUT:
                    // TODO
                    break;
            }
            if (geometry == UIGeometry::INDEPENDENT) {
                float percNW = nw * 0.01f;
                float percNH = nh * 0.01f;
                float x = widget->getX().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                float y = widget->getY().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                widgetProps[widget] = {
                        geometry,
                        x, y, nw, nh, nw, nh,
                };
            } else {
                widgetProps[widget] = {
                        geometry,
                        0.0f, 0.0f, nw, nh, 0.0f, 0.0f,
                };
            }
        }

        // Second pass (location and size)
        for (const UIWidget* widget : widgets) {
            Property& props = widgetProps[widget];
            Property& parentProps = widgetProps[widget->getParent()];
            if (props.geometry == UIGeometry::INDEPENDENT) {
                props.x += parentProps.x;
                props.y += parentProps.y;
            } else {
                float percW = parentProps.w * 0.01f;
                float percH = parentProps.h * 0.01f;
                float percNW = props.nw * 0.01f;
                float percNH = props.nh * 0.01f;
                props.x = widget->getX().calc(1.0f, percW, percH, percNW, percNH) + parentProps.x;
                props.y = widget->getY().calc(1.0f, percW, percH, percNW, percNH) + parentProps.y;
                props.w = widget->getW().calc(1.0f, percW, percH, percNW, percNH);
                props.h = widget->getH().calc(1.0f, percW, percH, percNW, percNH);
            }
        }

        // Final pass (assign to node)
        for (const UIWidget* widget : widgets) {
            Property& props = widgetProps[widget];
            Node& node = this->nodes.at(widget->id);
            node.location = { props.x, props.y };
            node.size = { props.w, props.h };
        }
    }

    Vec4 UIManager::getCachedGeometry(const UIWidget* target) const {
        const Node* node = this->getNodeFromWidget(target);
        return { node->location.x, node->location.y, node->size.x, node->size.y };
    }

    bool UIManager::widgetContains(const UIWidget* widget, Vec2 pos) const {
        auto [x, y, w, h] = this->getCachedGeometry(widget);  // NOLINT
        return x < pos.x && y < pos.y && x + w > pos.x && y + h > pos.y;
    }

    UIWidget* UIManager::getTargetWidget(Vec2 pos) {
        this->tidy();
        std::vector<UIWidget*> widgets = this->getAllActiveWidgets();
        for (UIWidget* widget : std::ranges::reverse_view(std::views::all(widgets))) {
            if (this->widgetContains(widget, pos)) return widget;
        }
        return nullptr;
    }

    void UIManager::onClick(const EventArgsMouseButton& args) {
        UIWidget* widget = this->getTargetWidget(args.mousePos);

        // Bubble up
        for (UIWidget* current = widget; current; current = current->getParent()) {
            if (this->widgetContains(current, args.mousePos)) {
                current->getEventManager().emit(EventArgsUIOnClick(args, widget));
            }
        }
    }

    void UIManager::onHover(const EventArgsMouseMove& args) {
        UIWidget* widget = this->getTargetWidget(args.mousePos);

        // Bubble up
        std::vector<UIWidget*> newHoveredWidgets;
        std::unordered_set<UIWidget*> newHoveredWidgetSet;
        for (UIWidget* current = widget; current; current = current->getParent()) {
            if (this->widgetContains(current, args.mousePos)) {
                newHoveredWidgets.push_back(current);
                newHoveredWidgetSet.insert(current);
            }
        }

        // On exit
        for (UIWidget* current : this->hoveredWidgets) {
            if (!newHoveredWidgetSet.contains(current)) {
                current->getEventManager().emit(EventArgsUIOnExit(args, widget));
            }
        }

        // On enter
        for (UIWidget* current : newHoveredWidgets) {
            if (!this->hoveredWidgetSet.contains(current)) {
                current->getEventManager().emit(EventArgsUIOnEnter(args, widget));
            }
        }

        // On hover
        for (UIWidget* current : newHoveredWidgets) {
            current->getEventManager().emit(EventArgsUIOnHover(args, widget));
        }

        // Update hovered widgets
        this->hoveredWidgets = std::move(newHoveredWidgets);
        this->hoveredWidgetSet = std::move(newHoveredWidgetSet);
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
