#include <array>
#include <cmath>
#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include <corn/geometry.h>
#include <corn/media.h>
#include <corn/ui.h>
#include <corn/util/string_utils.h>
#include "camera_viewport_impl.h"
#include "font_impl.h"
#include "image_impl.h"
#include "interface_impl.h"
#include "interface_helper.h"
#include "text_render_impl.h"

namespace corn {
    Interface::InterfaceImpl::InterfaceImpl() : window(new sf::RenderWindow()) {}

    Interface::InterfaceImpl::~InterfaceImpl() {
        this->window->close();
        delete this->window;
    }

    Interface::Interface(const Game& game, std::unordered_map<Key, bool>& keyPressed)
            : game_(game), keyPressed_(keyPressed), impl_(new Interface::InterfaceImpl()) {}

    Interface::~Interface() {
        delete this->impl_;
    }

    void Interface::init() {
        if (this->impl_->window->isOpen()) {
            this->impl_->window->close();
        }
        const Config& config = this->game_.getConfig();
        sf::ContextSettings contextSettings;
        contextSettings.antialiasingLevel = config.antialiasing;

        this->impl_->window->create(
                sf::VideoMode((int)config.width, (int)config.height),
                config.title,
                cornMode2SfStyle(config.mode),
                contextSettings);

        if (config.icon) {
            Vec2f size = config.icon->getSize();
            this->impl_->window->setIcon(
                    (unsigned int)size.x, (unsigned int)size.y, config.icon->impl_->image.getPixelsPtr());
        }
    }

    Vec2f Interface::windowSize() const noexcept {
        sf::Vector2u size = this->impl_->window->getSize();
        return Vec2f {(float)size.x, (float)size.y};
    }

    void Interface::handleUserInput() const {
        sf::Event event{};
        while (this->impl_->window->pollEvent(event)) {
            switch (event.type) {
                case (sf::Event::Closed): {
                    EventArgsInputExit eventArgs;
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::MouseButtonPressed): {
                    EventArgsMouseButton eventArgs(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::DOWN,
                            Vec2f(event.mouseButton.x, event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    // Only emit the event to the top scene if not caught by UI
                    if (!this->game_.getTopScene()->getUIManager().onClick(eventArgs)) {
                        EventArgsWorldMouseButton worldEventArgs(
                                sfInput2CornInput(event.mouseButton.button), ButtonEvent::DOWN,
                                Vec2f(event.mouseButton.x, event.mouseButton.y));
                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                    }
                    break;
                }
                case (sf::Event::MouseButtonReleased): {
                    EventArgsMouseButton eventArgs(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
                            Vec2f(event.mouseButton.x, event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    // Only emit the world event if not caught by UI
                    if (!this->game_.getTopScene()->getUIManager().onClick(eventArgs)) {
                        EventArgsWorldMouseButton worldEventArgs(
                                sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
                                Vec2f(event.mouseButton.x, event.mouseButton.y));
                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                    }
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventArgsMouseMove eventArgs(
                            Vec2f(event.mouseMove.x, event.mouseMove.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    // Only emit the world event if not caught by UI
                    if (!this->game_.getTopScene()->getUIManager().onHover(eventArgs)) {
                        EventArgsWorldMouseMove worldEventArgs(
                                Vec2f(event.mouseMove.x, event.mouseMove.y));
                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                    }
                    break;
                }
                case (sf::Event::MouseWheelScrolled): {
                    EventArgsMouseScroll eventArgs(
                            event.mouseWheelScroll.delta,
                            Vec2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    // Only emit the world event if not caught by UI
                    if (!this->game_.getTopScene()->getUIManager().onScroll(eventArgs)) {
                        EventArgsWorldMouseScroll worldEventArgs(
                                event.mouseWheelScroll.delta,
                                Vec2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y));
                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                    }
                    break;
                }
                case (sf::Event::KeyPressed): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
                    if (this->keyPressed_[key]) break;
                    this->keyPressed_[key] = true;
                    EventArgsKeyboard eventArgs(
                            key, ButtonEvent::DOWN,
                            (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
                            Vec2f(event.mouseButton.x, event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    // Only emit the world event if not caught by UI
                    if (!this->game_.getTopScene()->getUIManager().onKeyboard(eventArgs)) {
                        EventArgsWorldKeyboard worldEventArgs(
                                key, ButtonEvent::DOWN,
                                (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
                                Vec2f(event.mouseButton.x, event.mouseButton.y));
                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                    }
                    break;
                }
                case (sf::Event::KeyReleased): {
                    sf::Event::KeyEvent keyEvent = event.key;
                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
                    if (!this->keyPressed_[key]) break;
                    this->keyPressed_[key] = false;
                    EventArgsKeyboard eventArgs(
                            key, ButtonEvent::UP,
                            (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
                            Vec2f(event.mouseButton.x, event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    // Only emit the world event if not caught by UI
                    if (!this->game_.getTopScene()->getUIManager().onKeyboard(eventArgs)) {
                        EventArgsWorldKeyboard worldEventArgs(
                                key, ButtonEvent::UP,
                                (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
                                Vec2f(event.mouseButton.x, event.mouseButton.y));
                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                    }
                    break;
                }
                case (sf::Event::TextEntered): {
                    EventArgsTextEntered eventArgs(
                            event.text.unicode, unicodeToUTF8(event.text.unicode));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    this->game_.getTopScene()->getUIManager().onTextEntered(eventArgs);
                    break;
                }
                default:
                    break;
            }
        }
    }

    void Interface::clear() {
        const auto [r, g, b] = this->game_.getConfig().background.getRGB();
        this->impl_->window->clear(sf::Color(r, g, b));
    }

    void Interface::render(Scene* scene) {
        // Clear the screen
        this->clear();

        // Render Entities
        scene->getEntityManager().tidy();
        for (const CCamera* camera : scene->getEntityManager().getCameras()) {
            renderCamera(scene, camera);
        }

        // Render UI widgets
        this->renderUI(scene->getUIManager());

        this->impl_->window->setView(this->impl_->window->getDefaultView());
    }

    void Interface::renderDebugOverlay(size_t fps) {
        // Render dark background in the top left corner
        sf::RectangleShape overlay(sf::Vector2f(100, 30));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        this->impl_->window->draw(overlay);

        // Render FPS text
        sf::Text text;
        text.setFont(FontManager::instance().getDefault()->sffont);
        text.setString("FPS: " + std::to_string(fps));
        text.setCharacterSize(15);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, 6);
        this->impl_->window->draw(text);
    }

    void Interface::update() {
        this->impl_->window->display();
    }

    Transform2D Interface::getCameraTransformation(const CCamera* camera) const {
        // Calculate window size
        Vec2f windowSize = this->windowSize();
        Vec2f percentWindowSize = windowSize * 0.01f;

        // Calculate camera viewport and FOV
        Vec2f viewportSize(
                camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y));
        Vec2f fovSize(
                camera->fovW.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100) * (1 / camera->scale),
                camera->fovH.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100) * (1 / camera->scale));

        // Reset the camera viewport
        camera->viewport.impl_->setSize(viewportSize, this->game_.getConfig().antialiasing);
        const auto [r, g, b, a] = camera->background.getRGBA();  // NOLINT
        camera->viewport.impl_->texture.clear(sf::Color(r, g, b, a));

        // Calculate the transform of camera
        Transform2D worldTransform = camera->getEntity().getComponent<CTransform2D>()->getWorldTransform();
        Vec2f cameraScale = Vec2f(fovSize.x / viewportSize.x, fovSize.y / viewportSize.y);
        Deg cameraRotation = worldTransform.getRotationComponent();
        Vec2f cameraCenter = worldTransform.getTranslationComponent();
        return Transform2D(cameraCenter, cameraRotation, cameraScale) * Transform2D::translate(-viewportSize * 0.5f);
    }

    void Interface::renderCamera(Scene* scene, const CCamera* camera) {
        // Calculate window size
        Vec2f windowSize = this->windowSize();
        Vec2f percentWindowSize = windowSize * 0.01f;

        // Check if camera is active
        if (!camera->active) return;
        // TODO: 3D
        if (camera->cameraType == CameraType::_3D) return;
        // Check if camera has CTransform2D component
        if (!camera->getEntity().getComponent<CTransform2D>()) return;

        // Get camera transform
        Transform2D cameraTransform = this->getCameraTransformation(camera);

        // Render entities
        for (Entity* entity: scene->getEntityManager().getActiveEntitiesWith<CTransform2D>()) {
            auto transform = entity->getComponent<CTransform2D>();

            // Sprite
            auto sprite = entity->getComponent<CSprite>();
            if (sprite && sprite->active && sprite->image && sprite->image->impl_) {
                draw(*camera, *transform, *sprite, cameraTransform);
            }

            // Lines
            auto lines = entity->getComponent<CLines>();
            if (lines && lines->active && lines->vertices.size() > 1) {
                draw(*camera, *transform, *lines, cameraTransform);
            }

            // Polygon
            auto cPolygon = entity->getComponent<CPolygon>();
            if (cPolygon && cPolygon->active) {
                PolygonType polygonType = cPolygon->polygon.getType();
                if (polygonType != PolygonType::INVALID && polygonType != PolygonType::EMPTY) {
                    draw(*camera, *transform, *cPolygon, cameraTransform);
                }
            }

            // Text
            auto text = entity->getComponent<CText>();
            if (text && text->active) {
                draw(*camera, *transform, *text, cameraTransform);
            }
        }
        camera->viewport.impl_->texture.display();

        // Render camera onto the window
        float x = camera->viewport.x.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
        float y = camera->viewport.y.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
        sf::View view(sf::FloatRect(-x, -y, windowSize.x, windowSize.y));
        sf::Sprite cameraSprite(camera->viewport.impl_->texture.getTexture());
        cameraSprite.setColor(sf::Color(255, 255, 255, camera->opacity));
        this->impl_->window->setView(view);
        this->impl_->window->draw(cameraSprite);
    }

    void Interface::renderUI(UIManager& uiManager) {
        // Calculate window size
        Vec2f windowSize = this->windowSize();

        // Resolve UI widget location and size
        uiManager.tidy();
        uiManager.calcGeometry(windowSize);
        // Guaranteed to be from parent to children
        std::vector<UIWidget*> widgets = uiManager.getAllActiveWidgets();

        // Opacity & viewport
        std::unordered_map<const UIWidget*, float> opacities;
        std::unordered_map<const UIWidget*, std::pair<Vec2f, Vec2f>> subviewports;
        opacities[nullptr] = 1.0f;
        subviewports[nullptr] = { Vec2f::O(), windowSize };

        // Render
        for (UIWidget* widget : widgets) {
            // Find opacity
            UIWidget* parent = widget->getParent();
            opacities[widget] = opacities[parent] * (float)widget->getOpacity() / 255.0f;

            // Find geometry
            Vec4f widgetGeometry = uiManager.getCachedGeometry(widget);
            float x = widgetGeometry[0];
            float y = widgetGeometry[1];
            float w = widgetGeometry[2];
            float h = widgetGeometry[3];

            // Set view
            auto [vpul, vpbr] = subviewports[parent];
            sf::View view({
                vpul.x,
                vpul.y,
                vpbr.x - vpul.x,
                vpbr.y - vpul.y
            });
            view.setViewport({
                vpul.x / windowSize.x,
                vpul.y / windowSize.y,
                (vpbr.x - vpul.x) / windowSize.x,
                (vpbr.y - vpul.y) / windowSize.y
            });
            this->impl_->window->setView(view);

            // Update children viewport
            switch (widget->getOverflow()) {
                case UIOverflow::DISPLAY:
                    break;
                case UIOverflow::HIDDEN:
                    vpul.x = std::max(vpul.x, x);
                    vpul.y = std::max(vpul.y, y);
                    vpbr.x = std::min(vpbr.x, x + w);
                    vpbr.y = std::min(vpbr.y, y + h);
                    break;
            }
            subviewports[widget] = { vpul, vpbr };

            // Render the background
            sf::RectangleShape boundingRect(sf::Vector2f(w, h));
            boundingRect.setPosition(x, y);
            const auto [r, g, b, a] = widget->getBackground().getRGBA();
            boundingRect.setFillColor(sf::Color(r, g, b, (unsigned char)((float)a * opacities[widget])));
            this->impl_->window->draw(boundingRect);

            // Render the widget
            switch (widget->getType()) {
                case UIType::PANEL:
                    break;
                case UIType::LABEL: {
                    auto* uiLabel = dynamic_cast<UILabel*>(widget);
                    TextRender& textRender = uiLabel->getTextRender();
                    // Fit to width limit (if any)
                    textRender.setWidth(std::round(w));
                    // Render
                    float segX = x, segY = y;
                    for (TextRenderImpl::Line& line : textRender.impl_->lines) {
                        // alignment
                        switch (textRender.getTextAlign()) {
                            case TextAlign::LEFT:
                                segX = x;
                                break;
                            case TextAlign::CENTER:
                                segX = x + (w - line.size.x) / 2;
                                break;
                            case TextAlign::RIGHT:
                                segX = x + w - line.size.x;
                                break;
                        }

                        for (const auto& [text, color] : line.contents) {
                            const auto [segR, segG, segB, segA] = color.getRGBA(); // NOLINT
                            auto& mutText = const_cast<sf::Text&>(text);
                            mutText.setPosition(std::round(segX), std::round(segY + textRender.getLinePadding()));
                            mutText.setFillColor(sf::Color(
                                    segR, segG, segB, (unsigned char) ((float) segA * opacities[widget])));
                            this->impl_->window->draw(text);
                            segX += text.getLocalBounds().width;
                        }
                        segX = x;
                        segY += line.size.y;
                    }
                    break;
                }
                case UIType::IMAGE: {
                    const auto* uiImage = dynamic_cast<const UIImage*>(widget);
                    const Image* image = uiImage->getImage();
                    if (!image || !image->impl_) break;
                    // Scale image
                    Vec2f size = image->getSize();
                    Vec2f totalScale(size.x != 0.0f ? w / size.x : 1, size.y != 0.0f ? h / size.y : 1);
                    switch (image->impl_->type) {
                        case ImageType::SVG: {
                            image->impl_->rasterize(totalScale, true);
                            image->impl_->sfSprite.setOrigin(0, 0);
                            image->impl_->sfSprite.setPosition(x, y);
                            break;
                        }
                        case ImageType::PNG:
                        case ImageType::JPEG:
                        case ImageType::UNKNOWN: {
                            // Scale to fit the widget
                            image->impl_->sfSprite.setOrigin(0, 0);
                            image->impl_->sfSprite.setPosition(x, y);
                            image->impl_->sfSprite.setScale(totalScale.x, totalScale.y);
                            break;
                        }
                    }
                    this->impl_->window->draw(image->impl_->sfSprite);
                    break;
                }
            }
        }
    }
}
