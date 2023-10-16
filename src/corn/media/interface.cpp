#include <corn/event/event_manager.h>
#include <corn/media/interface.h>
#include <corn/util/geometry.h>
#include "image_impl.h"
#include "interface_impl.h"
#include "interface_helper.h"

namespace corn {
    std::unordered_map<Key, bool> Interface::keyPressed = std::unordered_map<Key, bool>();

    InterfaceImpl::InterfaceImpl(): window(new sf::RenderWindow()) {}

    InterfaceImpl::~InterfaceImpl() {
        this->window->close();
        delete this->window;
    }

    Interface::Interface(const Config* config): config(config), interfaceImpl(new InterfaceImpl()) {}

    Interface::~Interface() {
        delete this->interfaceImpl;
    }

    const InterfaceImpl& Interface::impl() const {
        return *this->interfaceImpl;
    }

    void Interface::init() {
        this->interfaceImpl->window->create(
                sf::VideoMode(this->config->width, this->config->height),
                this->config->title,
                cornMode2SfStyle(this->config->mode));
    }

    void Interface::handleUserInput() {  // TODO: change this
        sf::Event event{};
        while (this->interfaceImpl->window->pollEvent(event)) {
            switch (event.type) {
                case (sf::Event::Closed): {
                    EventManager::instance().emit(EventArgsExit());
                    break;
                }
                case (sf::Event::MouseButtonPressed): {
                    EventManager::instance().emit(EventArgsMouseButton(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::DOWN,
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::MouseButtonReleased): {
                    EventManager::instance().emit(EventArgsMouseButton(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::MouseWheelScrolled): {
                    EventManager::instance().emit(EventArgsMouseScroll(
                            event.mouseWheelScroll.delta,
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventManager::instance().emit(EventArgsMouseMove(
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::KeyPressed): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
                    if (Interface::keyPressed[key]) break;
                    Interface::keyPressed[key] = true;
                    EventManager::instance().emit(EventArgsKeyboard(key, ButtonEvent::DOWN,
                            (keyEvent.system << 3) + (keyEvent.control << 2)
                            + (keyEvent.system << 1) + (keyEvent.system << 3),
                            Vec2(event.mouseButton.x, event.mouseButton.y)));
                    break;
                }
                case (sf::Event::KeyReleased): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
                    if (!Interface::keyPressed[key]) break;
                    Interface::keyPressed[key] = false;
                    EventManager::instance().emit(EventArgsKeyboard(key, ButtonEvent::UP ,
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

    const std::unordered_map<Key, bool>& Interface::getKeyPressed() {
        return Interface::keyPressed;
    }

    void Interface::clear() {
        auto [r, g, b] = this->config->background.getRGB();
        this->interfaceImpl->window->clear(sf::Color(r, g, b));
    }

    void Interface::render(Scene* scene) {
        this->clear();
        scene->entityManager.tidy();
        for (Entity* entity : scene->entityManager.getActiveEntitiesWith<CTransform2D, CSprite>()) {
            auto trans = entity->getComponent<CTransform2D>();
            auto sprite = entity->getComponent<CSprite>();
            auto [x, y] = trans->worldLocation();
            sprite->image->impl().sfSprite->setPosition((float)x, (float)y);
            this->interfaceImpl->window->draw(*sprite->image->impl().sfSprite);
        }
    }

    void Interface::update() {
        this->interfaceImpl->window->display();
    }
}
