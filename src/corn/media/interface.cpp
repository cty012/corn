#include <ranges>
#include <corn/core/game.h>
#include <corn/ecs/component.h>
#include <corn/event/event_manager.h>
#include <corn/geometry/vec2.h>
#include <corn/media/interface.h>
#include <corn/ui/ui_label.h>
#include "camera_viewport_impl.h"
#include "font_impl.h"
#include "image_impl.h"
#include "interface_impl.h"
#include "interface_helper.h"
#include "rich_text_impl.h"

namespace corn {
    std::unordered_map<Key, bool> Interface::keyPressed = std::unordered_map<Key, bool>();

    InterfaceImpl::InterfaceImpl(): window(new sf::RenderWindow()) {}

    InterfaceImpl::~InterfaceImpl() {
        this->window->close();
        delete this->window;
    }

    CameraViewportImpl* InterfaceImpl::getCameraViewportImpl(const CameraViewport& cameraViewport) {
        return cameraViewport.impl;
    }

    ImageImpl* InterfaceImpl::getImageImpl(const Image& image) {
        return image.impl;
    }

    Interface::Interface(const Game& game): game(game), config(game.getConfig()), impl(new InterfaceImpl()) {}

    Interface::~Interface() {
        delete this->impl;
    }

    void Interface::init() {
        sf::ContextSettings contextSettings;
        contextSettings.antialiasingLevel = this->config.antialiasing;
        this->impl->window->create(
                sf::VideoMode((int)this->config.width, (int)this->config.height),
                this->config.title,
                cornMode2SfStyle(this->config.mode),
                contextSettings);
    }

    Vec2 Interface::windowSize() const {
        sf::Vector2u size = this->impl->window->getSize();
        return {(float)size.x, (float)size.y};
    }

    void Interface::handleUserInput() const {  // TODO: change this
        sf::Event event{};
        while (this->impl->window->pollEvent(event)) {
            switch (event.type) {
                case (sf::Event::Closed): {
                    EventArgsExit eventArgs;
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::MouseButtonPressed): {
                    EventArgsMouseButton eventArgs(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::DOWN,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::MouseButtonReleased): {
                    EventArgsMouseButton eventArgs(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::MouseWheelScrolled): {
                    EventArgsMouseScroll eventArgs(
                            event.mouseWheelScroll.delta,
                            Vec2((float)event.mouseWheelScroll.x, (float)event.mouseWheelScroll.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventArgsMouseMove eventArgs(
                            Vec2((float)event.mouseMove.x, (float)event.mouseMove.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::KeyPressed): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
                    if (Interface::keyPressed[key]) break;
                    Interface::keyPressed[key] = true;
                    EventArgsKeyboard eventArgs(
                            key, ButtonEvent::DOWN,
                            (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::KeyReleased): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
                    if (!Interface::keyPressed[key]) break;
                    Interface::keyPressed[key] = false;
                    EventArgsKeyboard eventArgs(
                            key, ButtonEvent::UP,
                            (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
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
        auto [r, g, b] = this->config.background.getRGB();
        this->impl->window->clear(sf::Color(r, g, b));
    }

    bool renderCamera(Scene* scene, const CCamera* camera, const Config& config, const Vec2& percentWindowSize) {
        // Check if camera is active
        if (!camera->active) return false;

        // TODO: 3D
        if (camera->cameraType == CameraType::_3D) return false;

        // Check if camera has CTransform2D component
        auto cameraTransform = camera->getEntity().getComponent<CTransform2D>();
        if (!cameraTransform) return false;

        // Calculate camera viewport and FOV
        Vec2 viewportSize(camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                          camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y));
        Vec2 fovSize(camera->fovW.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100),
                     camera->fovH.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100));
        sf::Transform stretchTransform;
        stretchTransform.scale(viewportSize.x / fovSize.x, viewportSize.y / fovSize.y);

        // Reset the camera viewport
        CameraViewportImpl* viewportImpl = InterfaceImpl::getCameraViewportImpl(camera->viewport);
        viewportImpl->setSize(viewportSize, config.antialiasing);
        auto [r, g, b, a] = camera->background.getRGBA();
        viewportImpl->texture.clear(sf::Color(r, g, b, a));

        // Calculate center of camera
        Vec2 cameraCenter = cameraTransform->location + camera->anchor.vec2();
        Vec2 cameraTL = cameraCenter - fovSize * 0.5;

        // Render entities
        for (Entity* entity: scene->getEntityManager().getActiveEntitiesWith<CTransform2D, CSprite>()) {
            auto transform = entity->getComponent<CTransform2D>();
            auto sprite = entity->getComponent<CSprite>();
            if (!sprite->visible) continue;

            auto [worldLocation, worldRotation] = transform->worldTransform();
            auto [ancX, ancY] = worldLocation - cameraTL;
            auto [locX, locY] = sprite->topLeft;
            ImageImpl* imageImpl = InterfaceImpl::getImageImpl(*sprite->image);
            imageImpl->sfSprite->setOrigin(-locX, -locY);
            imageImpl->sfSprite->setPosition(ancX, ancY);
            imageImpl->sfSprite->setRotation(-worldRotation.get());
            viewportImpl->texture.draw(*imageImpl->sfSprite, stretchTransform);
        }

        viewportImpl->texture.display();
        return true;
    }

    void Interface::renderUI(UIManager& uiManager) {
        Vec2 windowSize = this->windowSize();
        uiManager.tidy();
        uiManager.calcGeometry(windowSize);
        std::vector<UIWidget*> widgets = uiManager.getAllActiveWidgets();

        // Find opacity
        std::unordered_map<const UIWidget*, float> opacities;
        opacities[nullptr] = 1.0f;

        // Render
        for (const UIWidget* widget : widgets) {
            // Find opacity
            UIWidget* parent = widget->getParent();
            opacities[widget] = opacities[parent] * (float)widget->opacity / 255.0f;

            // Find geometry
            auto [x, y, w, h] = uiManager.getCachedGeometry(widget);

            // Render the background
            sf::RectangleShape boundingRect(sf::Vector2f(w, h));
            boundingRect.setPosition(x, y);
            auto [r, g, b, a] = widget->background.getRGBA();
            boundingRect.setFillColor(sf::Color(r, g, b, (unsigned char)((float)a * opacities[widget])));
            this->impl->window->draw(boundingRect);

            // Render the widget
            switch (widget->type) {
                case UIType::PANEL:
                    break;
                case UIType::LABEL: {
                    const auto* label = dynamic_cast<const UILabel*>(widget);
                    float _x = x, _y = y;
                    for (RichText::Segment* segment: label->getText().segments) {
                        if (segment->style.font->state != FontState::LOADED) continue;
                        segment->text.setPosition(_x, _y);
                        auto [_r, _g, _b, _a] = segment->style.color.getRGBA();
                        segment->text.setFillColor(sf::Color(_r, _g, _b, (unsigned char)((float)_a * opacities[widget])));
                        this->impl->window->draw(segment->text);
                        _x += segment->text.getLocalBounds().width;  // TODO: Text wrap
                    }
                    break;
                }
                case UIType::IMAGE:
                    // TODO
                    break;
                case UIType::INPUT:
                    // TODO
                    break;
            }
        }
    }

    void Interface::render(Scene* scene) {
        Vec2 windowSize = this->windowSize();
        Vec2 percentWindowSize = windowSize * 0.01f;

        // Clear the screen
        this->clear();

        // Render Entities
        scene->getEntityManager().tidy();
        for (const CCamera* camera : scene->getEntityManager().cameras) {
            if (renderCamera(scene, camera, this->config, percentWindowSize)) {
                float x = camera->viewport.x.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
                float y = camera->viewport.y.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
                sf::View view(sf::FloatRect(-x, -y, windowSize.x, windowSize.y));
                sf::Sprite cameraSprite(InterfaceImpl::getCameraViewportImpl(camera->viewport)->texture.getTexture());
                cameraSprite.setColor(sf::Color(255, 255, 255, camera->opacity));
                this->impl->window->setView(view);
                this->impl->window->draw(cameraSprite);
            }
        }

        // Render UI widgets
        sf::View view(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
        this->impl->window->setView(view);
        this->renderUI(scene->getUIManager());

        this->impl->window->setView(this->impl->window->getDefaultView());
    }

    void Interface::update() {
        this->impl->window->display();
    }
}
