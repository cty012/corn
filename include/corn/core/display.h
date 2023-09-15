#pragma once

#include <SFML/Graphics.hpp>
#include <corn/core/scene.h>
#include <corn/util/config.h>

namespace corn {
    DisplayMode sfStyle2CornMode(unsigned int style);
    unsigned int cornMode2SfStyle(DisplayMode mode);

    class Display {
    public:
        explicit Display(Config* settings);
        ~Display();
        Display(const Display& other);
        Display& operator=(const Display& other);

        void init();
        void clear();
        void render(Scene* scene);
        void update();

    private:
        Config* config;
        sf::RenderWindow* window;
    };
}
