#pragma once

namespace corn {
    class UIManager;

    /**
     * @class UIElement
     * @brief Base class for all UI elements.
     *
     * All UI elements' lifetime are managed by the UI manager class.
     */
    struct UIElement {
        UIManager& uiManager;

        /// @brief Destroys the UI element itself.
        void destroy();
        /// @brief Visitor pattern. Calls the corresponding render method of Interface class.
        virtual void render() const = 0;

    protected:
        explicit UIElement(UIManager& uiManager);
        virtual ~UIElement() = default;
    };
}
