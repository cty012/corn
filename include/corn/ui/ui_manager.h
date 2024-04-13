#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <corn/event/event_manager.h>
#include <corn/geometry/vec2.h>
#include <corn/ui/ui_widget.h>

namespace corn {
    struct EventArgsMouseButton;
    struct EventArgsMouseMove;
    struct Vec4;

    template <typename T>
    concept WidgetType = std::derived_from<T, UIWidget>;

    /**
     * @class UIManager
     * @brief Manages all UI widgets.
     */
    class UIManager {
    public:
        friend class UIWidget;

        /**
         * @struct Node
         * @brief Tree node containing each widget.
         */
        struct Node {
            UIWidget* widget;                      ///< Widget stored in the node
            Node* parent;                          ///< Parent node
            std::vector<Node*> children;           ///< Child nodes
            Vec2 location;                         ///< Cached location of the widget
            Vec2 size;                             ///< Cached size of the widget
            /**
             * @brief Whether the node's children are sorted by their z-order (small to large)
             *
             * False means it must be sorted, and true means it might not be.
             */
            bool dirty;
            Node(UIWidget* widget, Node* parent) noexcept;
        };

        /// @brief Constructor.
        explicit UIManager(Scene& scene) noexcept;

        /// @brief Destructor.
        ~UIManager();

        /// @return The scene that owns this UI manager.
        [[nodiscard]] Scene& getScene() const noexcept;

        /// @return The game that contains this UI manager.
        [[nodiscard]] const Game* getGame() const noexcept;

        /**
         * @brief Create a UI widget and attach it to the UI manager.
         * @tparam T Type of the widget, must derive from UIWidget class.
         * @param name Name of the widget. Widgets can have the same name.
         * @param parent Parent widget to attach the new widget. If value is null, will attach to the root.
         * @param args Arguments for constructing the UI widget (excluding the first argument UIManager& uiManager).
         * @return Pointer to the UI widget created.
         */
        template <WidgetType T, typename... Args>
        T& createWidget(const std::string& name, const UIWidget* parent, Args&&... args);

        /**
         * @param id ID of the UI widget.
         * @return UI widget with the given ID, or null pointer if it doesn't exist.
         *
         * Acquiring the UI widget by ID is the only method to access a UI widget in O(1) time complexity. All other
         * methods require traversing the widget tree, which takes O(n) time.
         */
        [[nodiscard]] UIWidget* getWidgetByID(UIWidget::WidgetID id) const noexcept;

        /**
         * @param name Name of the UI widget.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first UI widget with the given name, or null pointer if it doesn't exist.
         */
        [[nodiscard]] UIWidget* getWidgetByName(
                const std::string& name, const UIWidget* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @param name Name of the UI widget.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All UI widgets with the given name.
         */
        [[nodiscard]] std::vector<UIWidget*> getWidgetsByName(
                const std::string& name, const UIWidget* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @param pred A predicate function that takes an Entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return The first Entity that satisfy the conditions given by filter. Null pointer if it doesn't exist.
         */
        UIWidget* getWidgetThat(
                const std::function<bool(const UIWidget*)>& pred, const UIWidget* parent = nullptr, bool recurse = true) const;

        /**
         * @param pred A predicate function that takes an Entity pointer and returns whether it satisfy the conditions.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All Entities that satisfy the conditions given by filter.
         */
        std::vector<UIWidget*> getWidgetsThat(
                const std::function<bool(const UIWidget*)>& pred, const UIWidget* parent = nullptr, bool recurse = true) const;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All UI widgets.
         */
        std::vector<UIWidget*> getAllWidgets(const UIWidget* parent = nullptr, bool recurse = true) const noexcept;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All active UI widgets. See `UIWidget::isActive()` for definition of active.
         */
        std::vector<UIWidget*> getAllActiveWidgets(const UIWidget* parent = nullptr, bool recurse = true) const noexcept;

        /// @brief Clears all UI widgets.
        void clear() noexcept;

        /// @brief Cleans up all dirty nodes. Auto-called before rendering.
        void tidy() noexcept;

        /**
         * @brief Calculate and cache the locations and sizes of all widgets.
         * @param windowSize Size of the window
         */
        void calcGeometry(Vec2 windowSize);

        /**
         * @param target The target UI widget.
         * @return The cached value indicating the x location (left), y location (top), width, and height of the target.
         */
        Vec4 getCachedGeometry(const UIWidget* widget) const noexcept;

    private:
        /**
         * @param widget The target UI widget.
         * @param pos A location on the screen.
         * @return Whether the location is inside the target widget.
         */
        bool widgetContains(const UIWidget* widget, Vec2 pos) const noexcept;

        /**
         * @param pos A location on the screen.
         * @return The widget containing the specified location.
         *
         * Since there can be multiple widgets containing the same location, we only consider the widget on the top
         * along the z-axis.
         */
        UIWidget* getTargetWidget(Vec2 pos) noexcept;

        /**
         * @brief Propagates the mouse-click event to relevant UI widgets.
         * @param args Event to be propagated.
         *
         * The event will first be emitted to the widget returned by the getTargetWidget function. Then, it is
         * propagated to its parent, who will receive the event if and only if it contains this location. Regardless of
         * the result, it continues to propagate to its parent until it reaches the root node.
         */
        void onClick(const EventArgsMouseButton& args) noexcept;

        /**
         * @brief Propagates the mouse-hover, mouse-enter, and mouse-exit events to relevant UI widgets.
         * @param args Event to be propagated.
         *
         * The event will first be emitted to the widget returned by the getTargetWidget function. Then, it is
         * propagated to its parent, who will receive the event if and only if it contains this location. Regardless of
         * the result, it continues to propagate to its parent until it reaches the root node.
         */
        void onHover(const EventArgsMouseMove& args) noexcept;

        /**
         * @brief Helper to UIManager::destroyWidget.
         * @param node The node to be destroyed.
         *
         * Destroys a node and the widget inside, as well as all descendant nodes, but does not modify parent node
         */
        void destroyNode(Node* node) noexcept;

        /**
         * @brief Destroys an widget. First destroys all children before destroying itself.
         * @param widget The widget to be destroyed.
         *
         * You should not use this function to destroy a widget. Use `widget.destroy()` instead.
         */
        void destroyWidget(UIWidget& widget) noexcept;

        /**
         * @brief Given a pointer to widget, return the node containing it.
         * @throw std::invalid_argument if parent is not a valid widget created by the widget manager.
         *
         * The two functions are the same, but one is the const version of the other.
         */
        /// @{
        const Node* getNodeFromWidget(const UIWidget* widget) const;
        Node* getNodeFromWidget(const UIWidget* widget);
        /// @}

        /**
         * @brief Helper to all getWidget/getWidgets functions.
         * @param pred A predicate function that takes an widget pointer and returns whether it satisfy the conditions.
         *             Set it to null pointer to disable it.
         * @param onlyActive Whether to only consider active widgets. See `UIWidget::isActiveInWorld()` for definition
         *                   of active.
         * @param limit Maximum number of widgets to match. If set to 0, will match as much as possible.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All widgets satisfying the given conditions.
         */
        [[nodiscard]] std::vector<UIWidget*> getWidgetsHelper(
                const std::function<bool(UIWidget*)>& pred, bool onlyActive, size_t limit,
                const UIWidget* parent, bool recurse) const;

        /// @brief The scene that owns this UI manager.
        Scene& scene_;

        /// @brief The root node (does not contain a widget).
        Node root_;

        /// @brief Quick access for finding nodes by widget ID (does not contain root)
        std::unordered_map<UIWidget::WidgetID, Node> nodes_;

        EventManager::ListenerID mousebtnEventID_, mousemvEventID_, zOrderEventID_;

        std::vector<UIWidget*> hoveredWidgets_;
        std::unordered_set<UIWidget*> hoveredWidgetSet_;
    };

    template<WidgetType T, typename... Args>
    T& UIManager::createWidget(const std::string& name, const UIWidget* parent, Args &&... args) {
        // Verify parent
        Node* parentNode = this->getNodeFromWidget(parent);

        // Create the widget
        static UIWidget::WidgetID widgetID = 0;
        while (this->nodes_.contains(widgetID)) {
            widgetID++;
        }
        T* widget = new T(widgetID, name, *this, std::forward<Args>(args)...);

        // Create the node
        this->nodes_.emplace(widget->getID(), Node(widget, parentNode));
        parentNode->children.push_back(&this->nodes_.at(widget->getID()));
        parentNode->dirty = true;

        return *widget;
    }
}
