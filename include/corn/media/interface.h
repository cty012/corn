#pragma once

#include <unordered_map>
#include <corn/core/input.h>
#include <corn/core/scene.h>
#include <corn/util/config.h>

namespace corn {
    class InterfaceImpl;

    class Interface {
    public:
        explicit Interface(const Game& game);
        ~Interface();
        Interface(const Interface& other) = delete;
        Interface& operator=(const Interface& other) = delete;

        void init();

        [[nodiscard]] Vec2 windowSize() const;

        void handleUserInput() const;
        static const std::unordered_map<Key, bool>& getKeyPressed();

        void clear();
        void render(Scene* scene);
        void update();

    private:
        const Game& game;
        const Config& config;
        InterfaceImpl* impl;
        static std::unordered_map<Key, bool> keyPressed;
    };
}
