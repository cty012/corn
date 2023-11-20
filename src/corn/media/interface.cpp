#include <corn/core/game.h>
#include <corn/ecs/component.h>
#include <corn/event/event_manager.h>
#include <corn/geometry/vec2.h>
#include <corn/geometry/vec4.h>
#include <corn/media/interface.h>
#include <corn/ui/ui_label.h>

#include <ranges>
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventArgsMouseMove eventArgs(
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
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
        std::vector<UIWidget*> widgets = uiManager.getAllActiveWidgets();

        struct Property {
            UIGeometry geometry;
            float x, y, nw, nh, w, h;
            float opacity;
        };
        std::unordered_map<const UIWidget*, Property> widgetProps;
        widgetProps[nullptr] = {
                UIGeometry::INDEPENDENT,
                0.0f, 0.0f, windowSize.x, windowSize.y, windowSize.x, windowSize.y,
                1.0f,
        };

        // First pass (natural size)
        for (const UIWidget* widget : std::ranges::reverse_view(widgets)) {
            UIGeometry geometry = widget->getGeometry();

            // Calculate natural size
            float nw = 0.0f, nh = 0.0f;
            switch (widget->type) {
                case UIType::PANEL: {
                    std::vector<UIWidget*> independentChildren = uiManager.getWidgetsThat(
                            [&widgetProps](const UIWidget* widget) {
                                return widget->active && widgetProps.at(widget).geometry == UIGeometry::INDEPENDENT;
                            },
                        widget, false);
                    // TODO: find max of children size
                    for (UIWidget* child : independentChildren) {
                        nw = std::max(nw, widgetProps[child].nw);
                        nh = std::max(nh, widgetProps[child].nh);
                    }
                    break;
                }
                case UIType::LABEL: {
                    for (RichText::Segment* segment: dynamic_cast<const UILabel*>(widget)->getText().segments) {
                        nw += segment->text.getLocalBounds().width;
                        nh = std::max(nh, segment->text.getLocalBounds().height);
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
            if (geometry == UIGeometry::INDEPENDENT) {
                float percNW = nw * 0.01f;
                float percNH = nh * 0.01f;
                float x = widget->getX().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                float y = widget->getY().calc(1.0f, 0.0f, 0.0f, percNW, percNH);
                widgetProps[widget] = {
                        geometry,
                        x, y, nw, nh, nw, nh,
                        1.0f,
                };
            } else {
                widgetProps[widget] = {
                        geometry,
                        0.0f, 0.0f, nw, nh, 0.0f, 0.0f,
                        1.0f,
                };
            }
        }

        // Second pass (location and size)
        for (const UIWidget* widget : widgets) {
            Property& props = widgetProps[widget];
            Property& parentProps = widgetProps[widget->getParent()];
            if (props.geometry == UIGeometry::INDEPENDENT) {
                props.x += parentProps.x;
                props.y += parentProps.y;
                props.opacity = parentProps.opacity * (float)widget->opacity / 255.0f;
            } else {
                float percW = parentProps.w * 0.01f;
                float percH = parentProps.h * 0.01f;
                float percNW = props.nw * 0.01f;
                float percNH = props.nh * 0.01f;
                props.x = widget->getX().calc(1.0f, percW, percH, percNW, percNH) + parentProps.x;
                props.y = widget->getY().calc(1.0f, percW, percH, percNW, percNH) + parentProps.y;
                props.w = widget->getW().calc(1.0f, percW, percH, percNW, percNH);
                props.h = widget->getH().calc(1.0f, percW, percH, percNW, percNH);
                props.opacity = parentProps.opacity * (float)widget->opacity / 255.0f;
            }
        }

        // Final pass (render)
        for (const UIWidget* widget : widgets) {
            Property& props = widgetProps[widget];

            // Render the background
            sf::RectangleShape boundingRect(sf::Vector2f(props.w, props.h));
            boundingRect.setPosition(props.x, props.y);
            auto [r, g, b, a] = widget->background.getRGBA();
            boundingRect.setFillColor(sf::Color(r, g, b, (unsigned char)((float)a * props.opacity)));
            this->impl->window->draw(boundingRect);

            // Render the widget
            switch (widget->type) {
                case UIType::PANEL:
                    break;
                case UIType::LABEL: {
                    const auto* label = dynamic_cast<const UILabel*>(widget);
                    float _x = props.x, _y = props.y;
                    for (RichText::Segment* segment: label->getText().segments) {
                        if (segment->style.font->state != FontState::LOADED) continue;
                        segment->text.setPosition(_x, _y);
                        auto [_r, _g, _b, _a] = segment->style.color.getRGBA();
                        segment->text.setFillColor(sf::Color(_r, _g, _b, (unsigned char)((float)_a * props.opacity)));
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
