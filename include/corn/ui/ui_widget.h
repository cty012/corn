#pragma once

#include <array>
#include <string>
#include <corn/event/event_manager.h>
#include <corn/util/color.h>
#include <corn/util/expression.h>

namespace corn {
    class UIManager;
    class Scene;
    class Game;

    enum class UIType {
        PANEL, LABEL, IMAGE, INPUT,
    };

    /**
     * @class UIGeometry
     * @brief Defines how the widget's geometric properties are calculated.
     *
     * DEPENDENT: Depends on its own natural size and parent's actual size.
     * INDEPENDENT: Only depend on its own natural size.
     * DEFAULT: Auto-detect geometry type. If expressions for location and size uses parent size, then DEPENDENT.
     *          Otherwise INDEPENDENT.
     */
    enum class UIGeometry {
        DEFAULT, DEPENDENT, INDEPENDENT,
    };

    /**
     * @class UIWidget
     * @brief Base class for all UI widgets.
     *
     * All UI widgets' lifetime are managed by the UI manager class.
     */
    class UIWidget {
    public:
        using WidgetID = size_t;
        // UIManager need access to ctor/dtor
        friend class UIManager;

        /// @brief Getter for the widget's type.
        [[nodiscard]] UIType getType() const;

        /// @brief Getter for the widget's ID.
        [[nodiscard]] WidgetID getID() const;

        /// @brief Getter for the widget's name.
        [[nodiscard]] const std::string& getName() const;

        /// @brief Setter for the widget's name.
        void setName(std::string name);

        /// @return Getter for the entity's active property.
        [[nodiscard]] bool isActive() const;

        /// @brief Setter for the entity's active property.
        void setActive(bool active);

        /**
         * @return Whether the entity is active in the world.
         *
         * An entity is active in the world if and only if itself and all its ancestors have property active set to
         * true.
         */
        [[nodiscard]] bool isActiveInWorld() const;

        /// @return The UI manager that owns this widget.
        [[nodiscard]] UIManager& getUIManager() const;

        /// @return The private event manager owned by this widget.
        [[nodiscard]] EventManager& getEventManager() const;

        /// @return The scene that contains this widget.
        [[nodiscard]] Scene& getScene() const;

        /// @return The game that contains this widget.
        [[nodiscard]] const Game* getGame() const;

        /// @brief Destroys the widget itself.
        void destroy();

        /// @return Get the parent Entity.
        [[nodiscard]] UIWidget* getParent() const;

        /// @return Actual geometric type of the widget. DEFAULT will resolve to either DEPENDENT or INDEPENDENT.
        UIGeometry getActualGeometry() const;

        // Getters & setters
        const Expression<5>& getX() const;
        const Expression<5>& getY() const;
        const Expression<5>& getW() const;
        const Expression<5>& getH() const;
        void setX(const std::string& expression);
        void setY(const std::string& expression);
        void setW(const std::string& expression);
        void setH(const std::string& expression);
        const Color& getBackground() const;
        unsigned char getOpacity() const;
        void setBackground(Color background);
        void setOpacity(unsigned char opacity);

    protected:
        UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager);
        virtual ~UIWidget();
        UIWidget(const UIWidget& other) = delete;
        UIWidget& operator=(const UIWidget& other) = delete;

    private:
        UIWidget(WidgetID id, std::string name, UIManager& uiManager);

        /// @brief The type of the widget. Default value is UIType::PANEL.
        const UIType type_;

        /**
         * @brief The unique ID of the widget.
         *
         * The unique ID starts from 0 and adds one for each new widget created within the same scene. After it reaches
         * the max value of WidgetID, it goes back to 0.
         */
        const WidgetID id_;

        /**
         * @brief The name of the widget.
         *
         * Unlike ID, the name is a mutable property assigned during creation. Multiple Entities are allowed to have
         * the same name.
         */
        std::string name_;

        /// @brief Indicates whether the widget is active.
        bool active_;

        /// @brief The UI manager that owns this widget.
        UIManager& uiManager_;

        /// @brief The private event manager owned by this widget.
        EventManager* eventManager_;

        /// @brief The geometric type of the widget. See @UIGeometry for details.
        UIGeometry geometry_;

        /// @brief The UI widget
        Expression<5> x_, y_, w_, h_;
        std::array<bool, 4> independent_;

        /// @brief The background color of the widget.
        Color background_;

        /// @brief The opacity of the widget and all children, on a scale of [0, 255].
        unsigned char opacity_;
    };
}
