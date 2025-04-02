#include <array>
#include <cmath>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
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
#include "../render/interface_helper.h"
#include "../render/text_render_impl.h"

namespace corn {
    static void glfwErrorCallback(int error, const char *description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    void onWindowFramebufferResize(bgfx::ViewId viewID, int fwidth, int fheight) {
        bgfx::reset((uint32_t)fwidth, (uint32_t)fheight, BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4);

        // Create an orthographic projection matrix mapping [0, fwidth] and [0, fheight] to clip space.
        float view[16];
        bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
        bx::Vec3 eye = { 0.0f, 0.0f, -1.0f };
        bx::mtxLookAt(view, eye, at);
        const bgfx::Caps* caps = bgfx::getCaps();
        float proj[16];
        (void)viewID; (void)caps; (void)proj;
        bx::mtxOrtho(
                proj,
                0.0f, (float)fwidth, (float)fheight, 0.0f,
                0.0f, 100.0f,
                0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewID, view, proj);
    }

    Interface::InterfaceImpl::InterfaceImpl()
            : window(nullptr), viewID(), width(), height(), fwidth(), fheight(), polygonShader() {}

    Interface::Interface(const Game& game, std::unordered_map<Key, bool>& keyPressed)
            : game_(game), keyPressed_(keyPressed), impl_(new Interface::InterfaceImpl()) {}

    Interface::~Interface() {
        this->impl_->polygonShader.destroy();
        bgfx::shutdown();
        glfwTerminate();
        delete this->impl_;
    }

    void Interface::init() {
        const Config& config = this->game_.getConfig();

        // Create a GLFW window without an OpenGL context.
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit()) return;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        switch (config.mode) {
            case DisplayMode::WINDOWED:
                glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
                this->impl_->window = glfwCreateWindow(
                        (int)config.width, (int)config.height,
                        config.title.c_str(),
                        nullptr, nullptr);
                break;
            case DisplayMode::WINDOWED_FIXED:
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                this->impl_->window = glfwCreateWindow(
                        (int)config.width, (int)config.height,
                        config.title.c_str(),
                        nullptr, nullptr);
                break;
            case DisplayMode::FULLSCREEN:
                this->impl_->window = glfwCreateWindow(
                        mode->width, mode->height,
                        config.title.c_str(),
                        monitor, nullptr);
                break;
            case DisplayMode::BORDERLESS:
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
                this->impl_->window = glfwCreateWindow(
                        mode->width, mode->height,
                        config.title.c_str(),
                        nullptr, nullptr);
                break;
        }
        if (!this->impl_->window) {
            throw std::runtime_error("Failed to create GLFW window.");
        }

        // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
        // Most graphics APIs must be used on the same thread that created the window.
        bgfx::renderFrame();

        // Initialize bgfx using the native window handle.
        bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        init.platformData.ndt = glfwGetX11Display();
	    init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
        init.platformData.nwh = glfwGetCocoaWindow(this->impl_->window);
        init.type = bgfx::RendererType::Metal;
        init.platformData.ndt = nullptr;
#elif BX_PLATFORM_WINDOWS
        init.platformData.nwh = glfwGetWin32Window(window);
#endif

        // Initialize bgfx with the window resolution.
        glfwGetWindowSize(this->impl_->window, &this->impl_->width, &this->impl_->height);
        glfwGetFramebufferSize(this->impl_->window, &this->impl_->fwidth, &this->impl_->fheight);
        init.resolution.width = (uint32_t)this->impl_->fwidth;
        init.resolution.height = (uint32_t)this->impl_->fheight;
        init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4;
        if (!bgfx::init(init)) {
            throw std::runtime_error("Failed to initialize bgfx.");
        }
        this->impl_->viewID = 0;
        onWindowFramebufferResize(this->impl_->viewID, this->impl_->fwidth, this->impl_->fheight);

        // Shaders
        this->impl_->polygonShader.loadEmbedded("vs_triangle", "fs_triangle");
    }

    Vec2f Interface::windowLogicalSize() const noexcept {
        return Vec2f(static_cast<float>(this->impl_->width), static_cast<float>(this->impl_->height));
    }

    Vec2f Interface::windowPhysicalSize() const noexcept {
        return Vec2f(static_cast<float>(this->impl_->fwidth), static_cast<float>(this->impl_->fheight));
    }

    float Interface::getHiDPIScale() const noexcept {
        if (this->impl_->window == nullptr) return 1.0f;
        return static_cast<float>(this->impl_->fwidth) / static_cast<float>(this->impl_->width);
    }

    void Interface::checkWindowResize() const noexcept {
        int oldFWidth = this->impl_->fwidth;
        int oldFHeight = this->impl_->fheight;
        glfwGetWindowSize(this->impl_->window, &this->impl_->width, &this->impl_->height);
        glfwGetFramebufferSize(this->impl_->window, &this->impl_->fwidth, &this->impl_->fheight);
        if (this->impl_->fwidth != oldFWidth || this->impl_->fheight != oldFHeight) {
            onWindowFramebufferResize(this->impl_->viewID, this->impl_->fwidth, this->impl_->fheight);
        }
    }

    void Interface::handleUserInput() const {
        (void)this->keyPressed_;
        glfwPollEvents();

        // Exit event
        if (glfwWindowShouldClose(this->impl_->window)) {
            EventArgsInputExit eventArgs;
            EventManager::instance().emit(eventArgs);
            this->game_.getTopScene()->getEventManager().emit(eventArgs);
        }

//        sf::Event event{}; todo
//        while (this->impl_->window->pollEvent(event)) {
//            switch (event.type) {
//                case (sf::Event::Closed): {
//                    EventArgsInputExit eventArgs;
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    break;
//                }
//                case (sf::Event::MouseButtonPressed): {
//                    EventArgsMouseButton eventArgs(
//                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::DOWN,
//                            Vec2f(event.mouseButton.x, event.mouseButton.y));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    // Only emit the event to the top scene if not caught by UI
//                    if (!this->game_.getTopScene()->getUIManager().onClick(eventArgs)) {
//                        EventArgsWorldMouseButton worldEventArgs(
//                                sfInput2CornInput(event.mouseButton.button), ButtonEvent::DOWN,
//                                Vec2f(event.mouseButton.x, event.mouseButton.y));
//                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
//                    }
//                    break;
//                }
//                case (sf::Event::MouseButtonReleased): {
//                    EventArgsMouseButton eventArgs(
//                            sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
//                            Vec2f(event.mouseButton.x, event.mouseButton.y));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    // Only emit the world event if not caught by UI
//                    if (!this->game_.getTopScene()->getUIManager().onClick(eventArgs)) {
//                        EventArgsWorldMouseButton worldEventArgs(
//                                sfInput2CornInput(event.mouseButton.button), ButtonEvent::UP,
//                                Vec2f(event.mouseButton.x, event.mouseButton.y));
//                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
//                    }
//                    break;
//                }
//                case sf::Event::MouseMoved: {
//                    EventArgsMouseMove eventArgs(
//                            Vec2f(event.mouseMove.x, event.mouseMove.y));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    // Only emit the world event if not caught by UI
//                    if (!this->game_.getTopScene()->getUIManager().onHover(eventArgs)) {
//                        EventArgsWorldMouseMove worldEventArgs(
//                                Vec2f(event.mouseMove.x, event.mouseMove.y));
//                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
//                    }
//                    break;
//                }
//                case (sf::Event::MouseWheelScrolled): {
//                    EventArgsMouseScroll eventArgs(
//                            event.mouseWheelScroll.delta,
//                            Vec2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    // Only emit the world event if not caught by UI
//                    if (!this->game_.getTopScene()->getUIManager().onScroll(eventArgs)) {
//                        EventArgsWorldMouseScroll worldEventArgs(
//                                event.mouseWheelScroll.delta,
//                                Vec2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y));
//                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
//                    }
//                    break;
//                }
//                case (sf::Event::KeyPressed): {
//                    sf::Event::KeyEvent keyEvent = event.key;
//                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
//                    if (this->keyPressed_[key]) break;
//                    this->keyPressed_[key] = true;
//                    EventArgsKeyboard eventArgs(
//                            key, ButtonEvent::DOWN,
//                            (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
//                            Vec2f(event.mouseButton.x, event.mouseButton.y));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    // Only emit the world event if not caught by UI
//                    if (!this->game_.getTopScene()->getUIManager().onKeyboard(eventArgs)) {
//                        EventArgsWorldKeyboard worldEventArgs(
//                                key, ButtonEvent::DOWN,
//                                (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
//                                Vec2f(event.mouseButton.x, event.mouseButton.y));
//                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
//                    }
//                    break;
//                }
//                case (sf::Event::KeyReleased): {
//                    sf::Event::KeyEvent keyEvent = event.key;
//                    Key key = sfInput2CornInput(keyEvent.code, keyEvent.scancode);
//                    if (!this->keyPressed_[key]) break;
//                    this->keyPressed_[key] = false;
//                    EventArgsKeyboard eventArgs(
//                            key, ButtonEvent::UP,
//                            (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
//                            Vec2f(event.mouseButton.x, event.mouseButton.y));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    // Only emit the world event if not caught by UI
//                    if (!this->game_.getTopScene()->getUIManager().onKeyboard(eventArgs)) {
//                        EventArgsWorldKeyboard worldEventArgs(
//                                key, ButtonEvent::UP,
//                                (keyEvent.system << 3) + (keyEvent.control << 2) + (keyEvent.alt << 1) + keyEvent.shift,
//                                Vec2f(event.mouseButton.x, event.mouseButton.y));
//                        this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
//                    }
//                    break;
//                }
//                case (sf::Event::TextEntered): {
//                    EventArgsTextEntered eventArgs(
//                            event.text.unicode, unicodeToUTF8(event.text.unicode));
//                    EventManager::instance().emit(eventArgs);
//                    this->game_.getTopScene()->getEventManager().emit(eventArgs);
//                    this->game_.getTopScene()->getUIManager().onTextEntered(eventArgs);
//                    break;
//                }
//                default:
//                    break;
//            }
//        }
    }

    void Interface::clear() {
        bgfx::setViewRect(this->impl_->viewID, 0, 0, this->impl_->fwidth, this->impl_->fheight);
        const auto [r, g, b, a] = this->game_.getConfig().background.getRGBA();
        bgfx::setViewClear(
                this->impl_->viewID,
                BGFX_CLEAR_COLOR,
                (uint32_t(r) << 24) | (uint32_t(g) << 16) | (uint32_t(b) << 8) | uint32_t(a));
        bgfx::touch(this->impl_->viewID);
    }

    void Interface::render(Scene* scene) {
        // Render Entities
        scene->getEntityManager().tidy();
        for (const CCamera* camera : scene->getEntityManager().getCameras()) {
            renderCamera2D(scene, camera);
        }

        // Render UI widgets
//        this->renderUI(scene->getUIManager());
    }

    void Interface::renderDebugOverlay(size_t fps) {
        // Render dark background in the top left corner
        sf::RectangleShape overlay(sf::Vector2f(100, 30));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
//        this->impl_->window->draw(overlay); todo

        // Render FPS text
        sf::Text text;
        text.setFont(FontManager::instance().getDefault()->sffont);
        text.setString("FPS: " + std::to_string(fps));
        text.setCharacterSize(15);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, 6);
//        this->impl_->window->draw(text); todo
    }

    void Interface::update() {
        bgfx::frame();
    }

    Transform2D Interface::getCameraTransformation(const CCamera* camera) const {
        // Calculate window size
        Vec2f windowSize = this->windowLogicalSize();
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

    void Interface::renderCamera2D(Scene* scene, const CCamera* camera) {
        // Calculate window size
        Vec2f windowSize = this->windowLogicalSize();
        Vec2f percentWindowSize = windowSize * 0.01f;
        float hidpiScale = this->getHiDPIScale();

        // Check if camera is active
        if (!camera->active) return;
        if (camera->cameraType == CameraType::_3D) return;
        // Check if camera has CTransform2D component
        if (!camera->getEntity().getComponent<CTransform2D>()) return;

        // Get camera transform
        Transform2D cameraTransform = this->getCameraTransformation(camera);
        Transform2D cameraTransformInv = cameraTransform.inv();
        Transform2D worldToCameraTransform = Transform2D::dilate(corn::Vec2f(hidpiScale, hidpiScale)) * cameraTransformInv;

        // Set viewport
        float x = camera->viewport.x.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
        float y = camera->viewport.y.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
        float w = camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
        float h = camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y);
        bgfx::setViewRect(
                this->impl_->viewID,
                (uint16_t)std::round(x * hidpiScale), (uint16_t)std::round(y * hidpiScale),
                (uint16_t)std::round(w * hidpiScale), (uint16_t)std::round(h * hidpiScale));
        const auto [r, g, b, a] = camera->background.getRGBA();  // NOLINT
        bgfx::setViewClear(
                this->impl_->viewID,
                BGFX_CLEAR_COLOR,
                (uint32_t(r) << 24) | (uint32_t(g) << 16) | (uint32_t(b) << 8) | uint32_t(a));
        bgfx::touch(this->impl_->viewID);

        // Render entities
        for (Entity* entity: scene->getEntityManager().getActiveEntitiesWith<CTransform2D>()) {
            auto cTransform = entity->getComponent<CTransform2D>();

            // Sprite
            auto sprite = entity->getComponent<CSprite>();
            if (sprite && sprite->active && sprite->image && sprite->image->impl_) {
                draw(*camera, *cTransform, *sprite, cameraTransform);
            }

            // Lines
            auto lines = entity->getComponent<CLines>();
            if (lines && lines->active && lines->vertices.size() > 1) {
                draw(*camera, *cTransform, *lines, cameraTransform);
            }

            // Polygon
            auto cPolygon = entity->getComponent<CPolygon>();
            if (cPolygon && cPolygon->active) {
                PolygonType polygonType = cPolygon->getPolygon().getType();
                if (polygonType != PolygonType::INVALID && polygonType != PolygonType::EMPTY) {
                    draw(this->impl_->viewID, *cTransform, *cPolygon, worldToCameraTransform, this->impl_->polygonShader);
                }
            }

            // Text
            auto text = entity->getComponent<CText>();
            if (text && text->active) {
                draw(*camera, *cTransform, *text, cameraTransform);
            }
        }
    }

    void Interface::renderUI(UIManager& uiManager) {
        // Calculate window size
        Vec2f windowSize = this->windowLogicalSize();

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
//            this->impl_->window->setView(view); todo

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
//            this->impl_->window->draw(boundingRect); todo

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
//                            this->impl_->window->draw(text); todo
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
//                    this->impl_->window->draw(image->impl_->sfSprite); todo
                    break;
                }
            }
        }
    }
}
