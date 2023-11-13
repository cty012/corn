#pragma once

#include <string>

namespace corn {
    class UIManager;

    enum class UIType {
        PANEL, LABEL, IMAGE, INPUT
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
        /// @brief Reference to the UI Manager that created this widget.
        UIManager& uiManager;

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

        /// @brief The opacity of the widget and all children, on a scale of [0, 255].
        unsigned char opacity;

        /// @brief Destroys the widget itself.
        void destroy();
        /// @brief Visitor pattern. Calls the corresponding render method of Interface class.
        virtual void render() const = 0;

    protected:
        UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager);
        virtual ~UIWidget() = default;

    private:
        UIWidget(WidgetID id, std::string name, UIManager& uiManager);
    };
}
