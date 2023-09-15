#include <corn/core/display.h>

namespace corn {
    DisplayMode sfStyle2CornMode(int style) {
        return style & 0b1000 ? DisplayMode::FULLSCREEN : DisplayMode::WINDOWED;
    }

    unsigned int cornMode2SfStyle(DisplayMode mode) {
        switch (mode) {
            case DisplayMode::FULLSCREEN:
                return 0b1101;
            case DisplayMode::WINDOWED:
                return 0b0101;
        }
    }

    Display::Display(Config *config): config(config), window(new sf::RenderWindow()) {}

    Display::~Display() {
        this->window->close();  // TODO
        delete this->window;
    }

    Display::Display(const Display &other) {
        this->config = other.config;
        this->window = other.window;
    }

    Display& Display::operator=(const Display &other) {
        if (this == &other) return *this;
        this->config = other.config;
        this->window = other.window;
        return *this;
    }

    void Display::init() {
        this->window->create(
                sf::VideoMode(this->config->width, this->config->height),
                this->config->title,
                cornMode2SfStyle(this->config->mode));

    }

    void Display::clear() {
        auto [r, g, b] = this->config->background.getRGB();
        this->window->clear(sf::Color(r, g, b));
    }

    void Display::render(Scene *scene) {
        this->clear();
        // TODO: render
    }

    void Display::update() {
        this->window->display();
    }
}
