#include <corn/ecs/component.h>
#include <corn/event/event_manager.h>
#include <corn/geometry/vec2.h>
#include <corn/media/interface.h>
#include "camera_viewport_impl.h"
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
        sf::ContextSettings contextSettings;
        contextSettings.antialiasingLevel = 16;
        this->interfaceImpl->window->create(
                sf::VideoMode(this->config->width, this->config->height),
                this->config->title,
                cornMode2SfStyle(this->config->mode),
                contextSettings);
    }

    Vec2 Interface::windowSize() const {
        sf::Vector2u size = this->interfaceImpl->window->getSize();
        return {(float)size.x, (float)size.y};
    }

    void Interface::handleUserInput() const {  // TODO: change this
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y)));
                    break;
                }
                case (sf::Event::MouseButtonReleased): {
                    EventManager::instance().emit(EventArgsMouseButton(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y)));
                    break;
                }
                case (sf::Event::MouseWheelScrolled): {
                    EventManager::instance().emit(EventArgsMouseScroll(
                            event.mouseWheelScroll.delta,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y)));
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventManager::instance().emit(EventArgsMouseMove(
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y)));
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y)));
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y)));
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

    bool renderCamera(Scene* scene, const CCamera* camera, const Vec2& percentWindowSize) {
        // Check if camera is active
        if (!camera->active) return false;

        // TODO: 3D
        if (camera->cameraType == CameraType::_3D) return false;

        // Check if camera has CTransform2D component
        auto cameraTransform = camera->entity.getComponent<CTransform2D>();
        if (!cameraTransform) return false;

        // Calculate camera viewport and FOV
        Vec2 viewportSize(camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                          camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y));
        Vec2 fovSize(camera->fovW.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100),
                     camera->fovH.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100));
        sf::Transform stretchTransform;
        stretchTransform.scale(viewportSize.x / fovSize.x, viewportSize.y / fovSize.y);

        // Reset the camera viewport
        camera->viewport.impl->setSize(viewportSize);
        auto [r, g, b, a] = camera->background.getRGBA();
        camera->viewport.impl->texture.clear(sf::Color(r, g, b, a));

        // Calculate center of camera
        Vec2 cameraCenter = cameraTransform->location + camera->anchor.vec2();
        Vec2 cameraTL = cameraCenter - fovSize.mult(0.5);

        // Render entities
        for (Entity* entity: scene->entityManager.getActiveEntitiesWith<CTransform2D, CSprite>()) {
            auto transform = entity->getComponent<CTransform2D>();
            auto sprite = entity->getComponent<CSprite>();
            if (!sprite->visible) continue;

            auto [worldLocation, worldRotation] = transform->worldTransform();
            auto [ancX, ancY] = worldLocation - cameraTL;
            auto [locX, locY] = sprite->topLeft;
            sprite->image->impl().sfSprite->setOrigin(-locX, -locY);
            sprite->image->impl().sfSprite->setPosition(ancX, ancY);
            sprite->image->impl().sfSprite->setRotation(-worldRotation.get());
            camera->viewport.impl->texture.draw(*sprite->image->impl().sfSprite, stretchTransform);
        }

        camera->viewport.impl->texture.display();
        return true;
    }

    void Interface::render(Scene* scene) {
        Vec2 windowSize = this->windowSize();
        Vec2 percentWindowSize = windowSize.mult(0.01);

        this->clear();
        scene->entityManager.tidy();
        for (const CCamera* camera : scene->entityManager.cameras) {
            if (renderCamera(scene, camera, percentWindowSize)) {
                float x = camera->viewport.x.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
                float y = camera->viewport.y.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
                sf::View view(sf::FloatRect(-x, -y, windowSize.x, windowSize.y));
                sf::Sprite cameraSprite(camera->viewport.impl->texture.getTexture());
                this->interfaceImpl->window->setView(view);
                this->interfaceImpl->window->draw(cameraSprite);
            }
        }

        this->interfaceImpl->window->setView(this->interfaceImpl->window->getDefaultView());
        this->interfaceImpl->window->display();
    }

    void Interface::update() {
        this->interfaceImpl->window->display();
    }
}
