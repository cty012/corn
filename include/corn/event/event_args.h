#pragma once

#include <string>
#include <corn/event/input.h>
#include <corn/geometry/vec2.h>

namespace corn {
    /**
     * @class EventArgs
     * @brief Base class of all events.
     *
     * To define a custom event, declare a class derived from EventArgs. Override the `EventArgs::type` function to
     * return a string unique to the event type.
     *
     * To emit an event, instantiate the class and emit it using the `EventManager::emit` function. All corresponding
     * listeners will be called.
     *
     * @example
     * ```
     * class EventArgsExample : public EventArgs {
     *     const char* type() const override { return "example"; }
     *     int param;
     * };
     * EventManager::instance().addListener("example", [](const EventArgs& args) {
     *     const auto& _args = dynamic_cast<const EventArgsExample&>(args);
     *     printf("%d\n", _args.param);
     * });
     * EventManager::instance().emit(EventArgsExample{ 10 });  // prints 10
     * ```
     *
     * @see EventManager
     */
    struct EventArgs {
        /// @return Type of the event. Different events must have different types.
        [[nodiscard]] virtual std::string type() const noexcept = 0;

        /// @brief Destructor.
        virtual ~EventArgs();
    };

    /**
     * @class EventArgsInputExit
     * @brief User closes game window.
     *
     * This event emits when the corresponding user input (e.g. clicking the exit button) is received. It does not
     * imply that the game actually exits.
     */
    struct EventArgsInputExit : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::input::exit"; }
    };

    /**
     * @class EventArgsExit
     * @brief Emit this event to exit the game.
     */
    struct EventArgsExit : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::exit"; }
    };

    /**
     * @class EventArgsKeyboard
     * @brief User interacts with the keyboard.
     */
    struct EventArgsKeyboard : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::input::keyboard"; }

        /// @brief The key that the user interacts with.
        Key key;

        /// @brief The type of the interaction.
        ButtonEvent status;

        /**
         * @brief Status of the modifier keys.
         *
         * modifiers & (1 << 3): SYS key
         * modifiers & (1 << 2): CTRL key
         * modifiers & (1 << 1): ALT key
         * modifiers & (1 << 0): SHIFT key
         */
        unsigned char modifiers;

        /// @brief Location of the mouse.
        Vec2 mousePos;

        /// @brief Constructor.
        EventArgsKeyboard(Key key, ButtonEvent status, unsigned char modifiers, const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsWorldKeyboard
     * @brief User interacts with the keyboard and is not caught by UI.
     */
    struct EventArgsWorldKeyboard : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::world::keyboard"; }

        /// @brief The key that the user interacts with.
        Key key;

        /// @brief The type of the interaction.
        ButtonEvent status;

        /**
         * @brief Status of the modifier keys.
         *
         * modifiers & (1 << 3): SYS key
         * modifiers & (1 << 2): CTRL key
         * modifiers & (1 << 1): ALT key
         * modifiers & (1 << 0): SHIFT key
         */
        unsigned char modifiers;

        /// @brief Location of the mouse.
        Vec2 mousePos;

        /// @brief Constructor.
        EventArgsWorldKeyboard(Key key, ButtonEvent status, unsigned char modifiers, const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsMouseButton
     * @brief User clicks a mouse button.
     */
    struct EventArgsMouseButton : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::input::mousebtn"; }

        /// @brief The mouse button that the user interacts with.
        Mouse mouse;

        /// @brief The type of the interaction.
        ButtonEvent status;

        /// @brief Location of the mouse.
        Vec2 mousePos;

        /// @brief Constructor.
        EventArgsMouseButton(Mouse mouse, ButtonEvent status, const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsWorldMouseButton
     * @brief User clicks a mouse button and is not caught by the UI.
     */
    struct EventArgsWorldMouseButton : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::world::mousebtn"; }

        /// @brief The mouse button that the user interacts with.
        Mouse mouse;

        /// @brief The type of the interaction.
        ButtonEvent status;

        /// @brief Location of the mouse.
        Vec2 mousePos;

        /// @brief Constructor.
        EventArgsWorldMouseButton(Mouse mouse, ButtonEvent status, const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsMouseMove
     * @brief User moves the cursor.
     */
    struct EventArgsMouseMove : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::input::mousemv"; }

        /// @brief Location of the cursor after the movement.
        Vec2 mousePos;

        /// @brief Constructor.
        explicit EventArgsMouseMove(const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsWorldMouseMove
     * @brief User moves the cursor and is not caught by the UI.
     */
    struct EventArgsWorldMouseMove : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::world::mousemv"; }

        /// @brief Location of the cursor after the movement.
        Vec2 mousePos;

        /// @brief Constructor.
        explicit EventArgsWorldMouseMove(const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsMouseScroll
     * @brief User scrolls the middle mouse button.
     */
    struct EventArgsMouseScroll : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::input::mousesc"; }

        /// @brief The amount that the user scrolls.
        float value;

        /// @brief Location of the mouse.
        Vec2 mousePos;

        /// @brief Constructor.
        EventArgsMouseScroll(float value, const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsWorldMouseScroll
     * @brief User scrolls the middle mouse button and is not caught by the UI.
     */
    struct EventArgsWorldMouseScroll : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::world::mousesc"; }

        /// @brief The amount that the user scrolls.
        float value;

        /// @brief Location of the mouse.
        Vec2 mousePos;

        /// @brief Constructor.
        EventArgsWorldMouseScroll(float value, const Vec2& mousePos) noexcept;
    };

    /**
     * @class EventArgsTextEntered
     * @brief User enters a text.
     */
    struct EventArgsTextEntered : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::input::text"; }

        /// @brief The unicode of the entered character.
        unsigned int unicode;

        /// @brief The entered character encoded in UTF-8.
        std::u8string character;

        /// @brief Constructor.
        EventArgsTextEntered(unsigned int unicode, std::u8string character) noexcept;
    };

    class Scene;
    enum class SceneOperation;

    /**
     * @class EventArgsScene
     * @brief Emit this event to replace the currently active scene.
     */
    struct EventArgsScene : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::game::scene"; }

        /// @brief Method to replace the active scene.
        SceneOperation op;

        /// @brief New active scene (or null pointer if removing the current scene).
        Scene* scene;

        /// @brief Constructor.
        EventArgsScene(SceneOperation op, Scene* scene) noexcept;
    };

    struct CBBox;

    /**
     * @class EventArgsCollision
     * @brief Emits when two colliders overlap.
     */
    struct EventArgsCollision : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::game::collision"; }

        /// @brief First collider component.
        CBBox* collider1;

        /// @brief Second collider component.
        CBBox* collider2;

        /// @brief Constructor.
        EventArgsCollision(CBBox* collider1, CBBox* collider2) noexcept;
    };

    class UIWidget;

    /**
     * @class EventArgsUIKeyboard
     * @brief Emits when a UI widget catches a keyboard event.
     */
    struct EventArgsUIKeyboard : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::keyboard"; }

        /// @brief The mouse click event that triggers this event.
        EventArgsKeyboard keyboardEvent;

        /// @brief Constructor.
        explicit EventArgsUIKeyboard(EventArgsKeyboard keyboardEvent) noexcept;
    };

    /**
     * @class EventArgsUIOnClick
     * @brief Emits when a UI widget is clicked.
     */
    struct EventArgsUIOnClick : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onclick"; }

        /// @brief The mouse click event that triggers this event.
        EventArgsMouseButton mousebtnEvent;

        /// @brief The UI widget clicked.
        UIWidget* target;

        /// @brief Constructor.
        EventArgsUIOnClick(EventArgsMouseButton mousebtnEvent, UIWidget* target) noexcept;
    };

    /**
     * @class EventArgsUIOnHover
     * @brief Emits when a UI widget is hovered over.
     */
    struct EventArgsUIOnHover : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onhover"; }

        /// @brief The mouse move event that triggers this event.
        EventArgsMouseMove mousemvEvent;

        /// @brief The UI widget hovered over.
        UIWidget* target;

        /// @brief Constructor.
        EventArgsUIOnHover(EventArgsMouseMove mousemvEvent, UIWidget* target) noexcept;
    };

    /**
     * @class EventArgsUIOnEnter
     * @brief Emits when the cursor enters the UI widget's region.
     */
    struct EventArgsUIOnEnter : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onenter"; }

        /// @brief The mouse move event that triggers this event.
        EventArgsMouseMove mousemvEvent;

        /// @brief The UI widget entered by the cursor.
        UIWidget* target;

        /// @brief Constructor.
        EventArgsUIOnEnter(EventArgsMouseMove mousemvEvent, UIWidget* target) noexcept;
    };

    /**
     * @class EventArgsUIOnExit
     * @brief Emits when the cursor exits the UI widget's region.
     */
    struct EventArgsUIOnExit : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onexit"; }

        /// @brief The mouse move event that triggers this event.
        EventArgsMouseMove mousemvEvent;

        /// @brief The UI widget exited by the cursor.
        UIWidget* target;

        /// @brief Constructor.
        EventArgsUIOnExit(EventArgsMouseMove mousemvEvent, UIWidget* target) noexcept;
    };

    /**
     * @class EventArgsUIOnScroll
     * @brief Emits when a UI widget is hovered when scrolling.
     */
    struct EventArgsUIOnScroll : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onscroll"; }

        /// @brief The mouse scroll event that triggers this event.
        EventArgsMouseScroll mousescEvent;

        /// @brief The UI widget hovered when scrolling.
        UIWidget* target;

        /// @brief Constructor.
        EventArgsUIOnScroll(EventArgsMouseScroll mousescEvent, UIWidget* target) noexcept;
    };

    /**
     * @class EventArgsUIOnFocus
     * @brief Emits when a UI widget is focused.
     */
    struct EventArgsUIOnFocus : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onfocus"; }

        /// @brief The UI widget being focused.
        UIWidget* target;

        /// @brief Constructor.
        explicit EventArgsUIOnFocus(UIWidget* target) noexcept;
    };

    /**
     * @class EventArgsUIOnUnfocus
     * @brief Emits when a UI widget is unfocused.
     */
    struct EventArgsUIOnUnfocus : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::ui::onunfocus"; }

        /// @brief The UI widget being unfocused.
        UIWidget* target;

        /// @brief Constructor.
        explicit EventArgsUIOnUnfocus(UIWidget* target) noexcept;
    };
}
