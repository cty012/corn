#pragma once

#include <array>
#include <string>
#include <corn/util/color.h>
#include <corn/util/expression.h>

namespace corn {
    class EventManager;
    class Game;
    class Scene;
    class UIManager;

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
     *
     * @see UIManager
     */
    class UIWidget {
    public:
        using WidgetID = size_t;
        // UIManager need access to ctor/dtor
        friend class UIManager;

        /// @brief Getter for the widget's type.
        [[nodiscard]] UIType getType() const noexcept;

        /// @brief Getter for the widget's ID.
        [[nodiscard]] WidgetID getID() const noexcept;

        /// @brief Getter for the widget's name.
        [[nodiscard]] const std::string& getName() const noexcept;

        /// @brief Setter for the widget's name.
        void setName(std::string name) noexcept;

        /// @return Getter for the entity's active property.
        [[nodiscard]] bool isActive() const noexcept;

        /// @brief Setter for the entity's active property.
        void setActive(bool active) noexcept;

        /**
         * @return Whether the entity is active in the world.
         *
         * An entity is active in the world if and only if itself and all its ancestors have property active set to
         * true.
         */
        [[nodiscard]] bool isActiveInWorld() const noexcept;

        /// @return The UI manager that owns this widget.
        [[nodiscard]] UIManager& getUIManager() const noexcept;

        /// @return The private event manager owned by this widget.
        [[nodiscard]] EventManager& getEventManager() const noexcept;

        /// @return The scene that contains this widget.
        [[nodiscard]] Scene& getScene() const noexcept;

        /// @return The game that contains this widget.
        [[nodiscard]] const Game* getGame() const noexcept;

        /// @brief Destroys the widget itself.
        void destroy() noexcept;

        /// @return Get the parent Entity.
        [[nodiscard]] UIWidget* getParent() const noexcept;

        /// @return Actual geometric type of the widget. DEFAULT will resolve to either DEPENDENT or INDEPENDENT.
        [[nodiscard]] UIGeometry getActualGeometry() const noexcept;

        /// @brief Getter of the z-order of the entity.
        [[nodiscard]] int getZOrder() const noexcept;

        /// @brief Setter of the z-order of the entity.
        void setZOrder(int zOrder) noexcept;

        // Other getters & setters
        [[nodiscard]] const Expression<5>& getX() const noexcept;
        void setX(const std::string& expression);
        [[nodiscard]] const Expression<5>& getY() const noexcept;
        void setY(const std::string& expression);
        [[nodiscard]] const Expression<5>& getW() const noexcept;
        void setW(const std::string& expression);
        [[nodiscard]] const Expression<5>& getH() const noexcept;
        void setH(const std::string& expression);
        [[nodiscard]] const Color& getBackground() const noexcept;
        void setBackground(Color background) noexcept;
        [[nodiscard]] unsigned char getOpacity() const noexcept;
        void setOpacity(unsigned char opacity) noexcept;

    protected:
        /// @brief Constructor.
        UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager) noexcept;

        /// @brief Destructor.
        virtual ~UIWidget();

        UIWidget(const UIWidget& other) = delete;
        UIWidget& operator=(const UIWidget& other) = delete;

    private:
        UIWidget(WidgetID id, std::string name, UIManager& uiManager) noexcept;

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

        /// @brief The geometry of the widget with respect to its parent and own natural size
        Expression<5> x_, y_, w_, h_;

        /// @brief Whether the four values x, y, w, and h are independent from the parent's geometry.
        std::array<bool, 4> independent_;

        /// @brief The background color of the widget.
        Color background_;

        /// @brief The opacity of the widget and all children, on a scale of [0, 255].
        unsigned char opacity_;

        /**
         * @brief Defines the order of the widgets in the z direction (in/out of the screen). A higher z-order means
         * closer to the top.
         */
        int zOrder_;
    };
}
