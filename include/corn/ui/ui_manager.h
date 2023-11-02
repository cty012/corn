#pragma once

#include <corn/ecs/entity_manager.h>
#include <corn/ui/ui_widget.h>
#include <corn/util/config.h>

namespace corn {
    template <typename T>
    concept WidgetType = std::derived_from<T, UIWidget>;

    /**
     * @class UIManager
     * @brief Manages all UI elements.
     */
    class UIManager {
    public:
        friend class UIWidget;

        /// @brief The scene that owns the manager.
        Scene& scene;

        /**
         * @struct Node
         * @brief Tree node containing each widget.
         */
        struct Node {
            UIWidget* widget;                           ///< Widget stored in the node
            Node* parent;                          ///< Parent node
            std::vector<Node*> children;           ///< Child nodes
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

    private:
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
         * @brief Given a pointer to Entity, return the Node containing it.
         * @throw std::invalid_argument if parent is not a valid Entity created by the Entity Manager.
         *
         * The two functions are the same, but one is the const version of the other.
         */
        const Node* getNodeFromWidget(const UIWidget* widget) const;
        Node* getNodeFromWidget(const UIWidget* widget);

        /// @brief The root node (does not contain a widget)
        Node root;
        /// @brief Quick access for finding nodes by widget ID (does not contain root)
        std::unordered_map<UIWidget::WidgetID, Node> nodes;
    };
}

#include <corn/ui/ui_manager_template.h>
