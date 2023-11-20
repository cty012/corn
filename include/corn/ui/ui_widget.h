#pragma once

#include <array>
#include <string>
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

        /// @brief Indicates whether the widget is active.
        bool active;

        /// @brief The type of the widget. Default value is UIType::PANEL.
        const UIType type;

        /**
         * @brief The unique ID of the widget.
         *
         * The unique ID starts from 0 and adds one for each new widget created within the same scene. After it reaches
         * the max value of WidgetID, it goes back to 0.
         */
        const WidgetID id;

        /**
         * @brief The name of the widget.
         *
         * Unlike ID, the name is a mutable property assigned during creation. Multiple Entities are allowed to have
         * the same name.
         */
        std::string name;

        /// @brief The geometric type of the widget. See @UIGeometry for details.
        UIGeometry geometry;

        /// @brief The background color of the widget.
        Color background;

        /// @brief The opacity of the widget and all children, on a scale of [0, 255].
        unsigned char opacity;

        /// @brief Destroys the widget itself.
        void destroy();

        /**
         * @return Whether the widget is active. An UI widget is active iff itself and all its ancestors have
         * UIWidget::active set to true.
         */
        [[nodiscard]] bool isActive() const;

        /// @return The UI manager that owns this widget.
        [[nodiscard]] UIManager& getUIManager() const;

        /// @return The scene that contains this widget.
        [[nodiscard]] Scene& getScene() const;

        /// @return The game that contains this widget.
        [[nodiscard]] const Game* getGame() const;

        /// @return Get the parent Entity.
        [[nodiscard]] UIWidget* getParent() const;

        /// @return Actual geometric type of the widget. DEFAULT will resolve to either DEPENDENT or INDEPENDENT.
        UIGeometry getGeometry() const;

        // Getters & setters
        const Expression<5>& getX() const;
        const Expression<5>& getY() const;
        const Expression<5>& getW() const;
        const Expression<5>& getH() const;
        void setX(const std::string& expression);
        void setY(const std::string& expression);
        void setW(const std::string& expression);
        void setH(const std::string& expression);

    protected:
        /// @brief The UI widget
        Expression<5> x, y, w, h;
        std::array<bool, 4> independent;

        UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager);
        virtual ~UIWidget();
        UIWidget(const UIWidget& other) = delete;
        UIWidget& operator=(const UIWidget& other) = delete;

    private:
        UIWidget(WidgetID id, std::string name, UIManager& uiManager);

        /// @brief The UI manager that owns this widget.
        UIManager& uiManager;
    };
}
