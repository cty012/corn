#pragma once

#include <unordered_map>
#include <corn/core/input.h>
#include <corn/core/scene.h>
#include <corn/util/config.h>

namespace corn {
    class InterfaceImpl;

    class Interface {
    public:
        explicit Interface(const Config* settings);
        ~Interface();
        Interface(const Interface& other) = delete;
        Interface& operator=(const Interface& other) = delete;

        [[nodiscard]] const InterfaceImpl& impl() const;

        void init();

        [[nodiscard]] Vec2 windowSize() const;

        void handleUserInput() const;
        static const std::unordered_map<Key, bool>& getKeyPressed();

        void clear();
        void render(Scene* scene);
        void update();

    private:
        const Config* config;
        InterfaceImpl* interfaceImpl;
        static std::unordered_map<Key, bool> keyPressed;
    };
}
