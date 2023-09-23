#pragma once

#include "corn/core/input.h"
#include "corn/core/scene.h"
#include "corn/util/config.h"

namespace sf {
    class RenderWindow;
}

namespace corn {
    class Interface {
    public:
        explicit Interface(const Config* settings);
        ~Interface();
        Interface(const Interface& other);
        Interface& operator=(const Interface& other);

        void init();

        void handleUserInput();

        void clear();
        void render(Scene* scene);
        void update();

    private:
        const Config* config;
        sf::RenderWindow* window;
    };
}
