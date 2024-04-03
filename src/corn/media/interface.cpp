#include <array>
#include <cmath>
#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include <corn/geometry.h>
#include <corn/media.h>
#include <corn/ui.h>
#include <corn/util/constants.h>
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
    }

    Vec2 Interface::windowSize() const noexcept {
        sf::Vector2u size = this->impl_->window->getSize();
        return {(float)size.x, (float)size.y};
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::MouseButtonReleased): {
                    EventArgsMouseButton eventArgs(
                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::MouseWheelScrolled): {
                    EventArgsMouseScroll eventArgs(
                            event.mouseWheelScroll.delta,
                            Vec2((float)event.mouseWheelScroll.x, (float)event.mouseWheelScroll.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case sf::Event::MouseMoved: {
                    EventArgsMouseMove eventArgs(
                            Vec2((float)event.mouseMove.x, (float)event.mouseMove.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
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
                            Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                case (sf::Event::TextEntered): {
                    EventArgsTextEntered eventArgs(
                            event.text.unicode, unicodeToUTF8(event.text.unicode));
                    EventManager::instance().emit(eventArgs);
                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
                    break;
                }
                default:
                    break;
            }
        }
    }

    void Interface::clear() {
        auto [r, g, b] = this->game_.getConfig().background.getRGB();
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

    void Interface::update() {
        this->impl_->window->display();
    }

    std::pair<Vec2, Vec2> Interface::getCameraTransformation(const CCamera* camera) const {
        // Calculate window size
        Vec2 windowSize = this->windowSize();
        Vec2 percentWindowSize = windowSize * 0.01f;

        // Calculate camera viewport and FOV
        Vec2 viewportSize(camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                          camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y));
        Vec2 fovSize(camera->fovW.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100),
                     camera->fovH.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100));

        // Reset the camera viewport
        camera->viewport.impl_->setSize(viewportSize, this->game_.getConfig().antialiasing);
        auto [r, g, b, a] = camera->background.getRGBA();
        camera->viewport.impl_->texture.clear(sf::Color(r, g, b, a));

        // Calculate location of camera
        Vec2 cameraCenter = camera->getEntity().getComponent<CTransform2D>()->location + camera->anchor.vec2();

        // Return the location and scale
        return { cameraCenter - fovSize * 0.5, { viewportSize.x / fovSize.x, viewportSize.y / fovSize.y } };
    }

    void Interface::renderCamera(Scene* scene, const CCamera* camera) {
        // Calculate window size
        Vec2 windowSize = this->windowSize();
        Vec2 percentWindowSize = windowSize * 0.01f;

        // Check if camera is active
        if (!camera->active) return;
        // TODO: 3D
        if (camera->cameraType == CameraType::_3D) return;
        // Check if camera has CTransform2D component
        if (!camera->getEntity().getComponent<CTransform2D>()) return;

        // Get camera transform
        auto [cameraOffset, cameraScale] = this->getCameraTransformation(camera);
        sf::Transform scaleTransform;
        scaleTransform.scale(cameraScale.x, cameraScale.y);

        // Helper functions
        auto drawLines =
        [&cameraOffset, &camera, &cameraScale, &scaleTransform]
        (CTransform2D* transform, const std::vector<Vec2>& vertices, float thickness, bool closed) -> void {
            auto [worldLocation, worldRotation] = transform->getWorldTransform();
            auto [ancX, ancY] = worldLocation - cameraOffset;

            for (size_t i = 0; (closed ? i : i + 1) < vertices.size(); i++) {
                auto [startX, startY] = vertices[i];
                auto [endX, endY] = vertices[(i + 1) % vertices.size()];
                float diffX = endX - startX, diffY = endY - startY;
                float length = std::sqrt(diffX * diffX + diffY * diffY);
                float angle = std::atan2(diffY, diffX) * 180.0f / (float)PI;

                sf::RectangleShape line;
                line.setSize(sf::Vector2f(length, thickness / cameraScale.norm()));
                line.setOrigin(0, 0);
                line.setPosition(ancX + startX, ancY + startY);
                line.setFillColor(sf::Color::Black);
                line.setRotation(-worldRotation.get() + angle);
                camera->viewport.impl_->texture.draw(line, scaleTransform);
            }
        };

        auto drawPolygon =
        [&cameraOffset, &camera, &scaleTransform]
        (CTransform2D* transform, CPolygon* polygon) -> void {
            auto [worldLocation, worldRotation] = transform->getWorldTransform();
            auto [ancX, ancY] = worldLocation - cameraOffset;

            const std::vector<std::array<Vec2, 3>>& triangles = polygon->getTriangles();
            sf::VertexArray varr(sf::Triangles, triangles.size() * 3);
            for (size_t i = 0; i < triangles.size(); i++) {
                for (size_t j = 0; j < 3; j++) {
                    varr[i * 3 + j].position = sf::Vector2f(ancX + triangles[i][j].x, ancY + triangles[i][j].y);
                    varr[i * 3 + j].color = sf::Color::Cyan;
                }
            }

            sf::Transform rotateTransform;
            rotateTransform.rotate(-worldRotation.get());

            camera->viewport.impl_->texture.draw(varr, rotateTransform * scaleTransform);
        };

        // Render entities
        for (Entity* entity: scene->getEntityManager().getActiveEntitiesWith<CTransform2D>()) {
            auto transform = entity->getComponent<CTransform2D>();

            // Sprite
            auto sprite = entity->getComponent<CSprite>();
            if (sprite && sprite->active && sprite->image && sprite->image->impl_) {
                auto [worldLocation, worldRotation] = transform->getWorldTransform();
                auto [ancX, ancY] = worldLocation - cameraOffset;
                auto [locX, locY] = sprite->location;
                sf::Sprite& sfSprite = sprite->image->impl_->sfSprite;
                sfSprite.setOrigin(-locX, -locY);
                sfSprite.setPosition(ancX, ancY);
                sfSprite.setRotation(-worldRotation.get());
                camera->viewport.impl_->texture.draw(sfSprite, scaleTransform);
            }

            // Polygon
            auto polygon = entity->getComponent<CPolygon>();
            if (polygon && polygon->active && !polygon->getVertices().empty()) {
                if (polygon->thickness > 0) {
                    for (const std::vector<Vec2>& arc : polygon->getVertices()) {
                        drawLines(transform, arc, polygon->thickness, true);
                    }
                } else {
                    drawPolygon(transform, polygon);
                }
            }

            // Lines
            auto lines = entity->getComponent<CLines>();
            if (lines && lines->active && lines->vertices.size() > 1) {
                drawLines(transform, lines->vertices, lines->thickness, lines->closed);
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
        Vec2 windowSize = this->windowSize();
        sf::View view(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
        this->impl_->window->setView(view);

        // Resolve UI widget location and size
        uiManager.tidy();
        uiManager.calcGeometry(windowSize);
        std::vector<UIWidget*> widgets = uiManager.getAllActiveWidgets();

        // Find opacity
        std::unordered_map<const UIWidget*, float> opacities;
        opacities[nullptr] = 1.0f;

        // Render
        for (UIWidget* widget : widgets) {
            // Find opacity
            UIWidget* parent = widget->getParent();
            opacities[widget] = opacities[parent] * (float)widget->getOpacity() / 255.0f;

            // Find geometry
            auto [x, y, w, h] = uiManager.getCachedGeometry(widget);  // NOLINT

            // Render the background
            sf::RectangleShape boundingRect(sf::Vector2f(w, h));
            boundingRect.setPosition(x, y);
            auto [r, g, b, a] = widget->getBackground().getRGBA();
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
                    for (TextRender::TextRenderImpl::Line& line : textRender.impl_->lines) {
                        for (const auto& [text, color] : line.contents) {
                            auto [segR, segG, segB, segA] = color.getRGBA();
                            auto& mutText = const_cast<sf::Text&>(text);
                            mutText.setPosition(segX, segY);
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
                    sf::Sprite& sfSprite = image->impl_->sfSprite;
                    sfSprite.setOrigin(0, 0);
                    sfSprite.setPosition(x, y);
                    // Scale image
                    Vec2 size = image->getSize();
                    Vec2 totalScale = image->impl_->scale * Vec2(size.x ? w / size.x : 1, size.y ? h / size.y : 1);
                    // Draw the image on the screen
                    sfSprite.setScale(totalScale.x, totalScale.y);
                    this->impl_->window->draw(sfSprite);
                    break;
                }
            }
        }
    }
}
