#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <corn/geometry/vec4.h>
#include <corn/ui/ui_widget.h>
#include <corn/util/config.h>

namespace corn {
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
             * False means it must be sorted, and true means it might not be
             */
            bool dirty;
            Node(UIWidget* widget, Node* parent);
        };

        explicit UIManager(Scene& scene);
        ~UIManager();

        /// @return The scene that owns this UI manager.
        [[nodiscard]] Scene& getScene() const;

        /// @return The game that contains this UI manager.
        [[nodiscard]] const Game* getGame() const;

        /**
         * @brief Create a UI widget and attach it to the UI manager.
         * @tparam T Type of the widget, must derive from UIWidget class.
         * @param name Name of the widget. Widgets can have the same name.
         * @param parent Parent widget to attach the new widget. If value is null, will attach to the root.
         * @param args Arguments for constructing the UI widget (excluding the first argument UIManager& uiManager).
         * @return Pointer to the UI widget created.
         */
        template <WidgetType T, typename... Args>
        T* createWidget(const std::string& name, const UIWidget* parent, Args&&... args);

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
        std::vector<UIWidget*> getAllWidgets(const UIWidget* parent = nullptr, bool recurse = true) const;

        /**
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All active UI widgets. See `UIWidget::isActive()` for definition of active.
         */
        std::vector<UIWidget*> getAllActiveWidgets(const UIWidget* parent = nullptr, bool recurse = true) const;

        /// @brief Cleans up all dirty nodes. Auto-called before rendering.
        void tidy();

        /// @brief Calculate and cache the locations and sizes of all widgets.
        void calcGeometry(Vec2 rootSize);

        Vec4 getCachedGeometry(const UIWidget* target) const;

    private:
        bool widgetContains(UIWidget* widget, Vec2 pos) const;
        UIWidget* getTargetWidget(Vec2 pos);
        void onClick(const EventArgsMouseButton& args);
        void onHover(const EventArgsMouseMove& args);

        /**
         * @brief Helper to UIManager::destroyWidget
         *
         * Destroys a node and the widget inside, as well as all descendant nodes, but does not modify parent node
         */
        void destroyNode(Node* node);

        /**
         * @brief Destroys an widget. First destroys all children before destroying itself.
         * @param widget The widget to be destroyed.
         *
         * You should not use this function to destroy a widget. Use `widget.destroy()` instead.
         */
        void destroyWidget(UIWidget& widget);

        /**
         * @brief Given a pointer to widget, return the Node containing it.
         * @throw std::invalid_argument if parent is not a valid widget created by the widget Manager.
         *
         * The two functions are the same, but one is the const version of the other.
         */
        const Node* getNodeFromWidget(const UIWidget* widget) const;
        Node* getNodeFromWidget(const UIWidget* widget);

        /**
         * @brief Helper to all getEntity/getEntities functions.
         * @param pred A predicate function that takes an Entity pointer and returns whether it satisfy the conditions.
         *             Set it to null pointer to disable it.
         * @param onlyActive Whether to only consider active entities. See `Entity::isActive()` for definition of
         *                   active.
         * @param limit Maximum number of entities to match. If set to 0, will match as much as possible.
         * @param parent Parent to start searching from.
         * @param recurse Also searches indirect descendants of parent if set to true.
         * @return All entities satisfying the given conditions.
         */
        std::vector<UIWidget*> getWidgetsHelper(
                const std::function<bool(UIWidget*)>& pred, bool onlyActive, size_t limit,
                const UIWidget* parent, bool recurse) const;

        /// @brief The scene that owns this UI manager.
        Scene& scene;

        /// @brief The root node (does not contain a widget)
        Node root;

        /// @brief Quick access for finding nodes by widget ID (does not contain root)
        std::unordered_map<UIWidget::WidgetID, Node> nodes;

        EventManager::ListenerID mousebtnEventID;
        EventManager::ListenerID mousemvEventID;
    };

    template<WidgetType T, typename... Args>
    T* UIManager::createWidget(const std::string& name, const UIWidget* parent, Args &&... args) {
        // Verify parent
        Node* parentNode = this->getNodeFromWidget(parent);

        // Create the widget
        static UIWidget::WidgetID widgetID = 0;
        while (this->nodes.contains(widgetID)) {
            widgetID++;
        }
        T* widget = new T(widgetID, name, *this, std::forward<Args>(args)...);

        // Create the node
        this->nodes.emplace(widget->id, Node(widget, parentNode));
        parentNode->children.push_back(&this->nodes.at(widget->id));
        parentNode->dirty = true;

        return widget;
    }
}
