#include <SFML/Graphics.hpp>
#include <corn/core/interface.h>
#include "interface_helper.h"
#include <corn/event/event_manager.h>
#include <corn/util/geometry.h>

namespace corn {
    Interface::Interface(const Config* config): config(config), window(new sf::RenderWindow()) {}

    Interface::~Interface() {
        this->window->close();
        delete this->window;
    }

    Interface::Interface(const Interface &other) {
        this->config = other.config;
        this->window = other.window;
    }

    Interface& Interface::operator=(const Interface &other) {
        if (this == &other) return *this;
        this->config = other.config;
        this->window = other.window;
        return *this;
    }

    void Interface::init() {
        this->window->create(
                sf::VideoMode(this->config->width, this->config->height),
                this->config->title,
                cornMode2SfStyle(this->config->mode));

    }

    void Interface::handleUserInput() {
        sf::Event event{};
        while (this->window->pollEvent(event)) {
            switch (event.type) {
                case (sf::Event::Closed): {
                    EventManager::instance().emit(EventArgsExit());
                    break;
                }
                case (sf::Event::MouseButtonPressed):
                case (sf::Event::MouseButtonReleased): {
                    EventManager::instance().emit(EventArgsMouse(
                            sfInput2CornInput(event.mouseButton.button),
                            event.type == sf::Event::MouseButtonPressed ? 1.0 : -1.0,
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::MouseWheelScrolled): {
                    EventManager::instance().emit(EventArgsMouse(
                            MouseEvent::MOVE,
                            event.mouseWheelScroll.delta,
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventManager::instance().emit(EventArgsMouse(
                            MouseEvent::MOVE,
                            0.0,
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::KeyPressed):
                case (sf::Event::KeyReleased): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    EventManager::instance().emit(EventArgsKeyboard(
                            sfInput2CornInput(keyEvent.code, keyEvent.scancode),
                            event.type == sf::Event::KeyPressed ? KeyEvent::PRESSED : KeyEvent::RELEASED ,
                            (keyEvent.system << 3) + (keyEvent.control << 2)
                                + (keyEvent.system << 1) + (keyEvent.system << 3),
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::TextEntered):
                    // TODO
                    break;
                default:
                    break;
            }
        }
    }

    void Interface::clear() {
        auto [r, g, b] = this->config->background.getRGB();
        this->window->clear(sf::Color(r, g, b));
    }

    void Interface::render(Scene *scene) {
        this->clear();
        // TODO: render
    }

    void Interface::update() {
        this->window->display();
    }
}
