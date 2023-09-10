#pragma once


#include <corn/event/input.h>

namespace corn {
    class EventArgs {
    public:
        /**
         * @return Type of the event
         */
        [[nodiscard]] virtual std::string type() const = 0;
    protected:
        std::string eventType;
    };

    class EventArgsKeyboard : public EventArgs {
    public:
        explicit EventArgsKeyboard(Key key);
        [[nodiscard]] std::string type() const override;
        [[nodiscard]] Key getKey() const;
    private:
        Key key;
    };

    class EventArgsMouse : public EventArgs {
    public:
        explicit EventArgsMouse(Mouse mouse);
        [[nodiscard]] std::string type() const override;
        [[nodiscard]] Mouse getMouse() const;
    private:
        Mouse mouse;
    };

    class Scene;
    enum class SceneOperation;

    class EventArgsScene : public EventArgs {
    public:
        EventArgsScene(SceneOperation op, Scene* scene);
        [[nodiscard]] std::string type() const override;
        [[nodiscard]] SceneOperation getOperation() const;
        [[nodiscard]] Scene* getScene() const;
    private:
        SceneOperation op;
        Scene* scene;
    };
}
