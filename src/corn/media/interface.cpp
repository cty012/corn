#include <array>
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
#include "interface_impl.h"
#include "../render/image_impl.h"
#include "../render/interface_helper.h"
#include "../render/polygon_renderer.h"

namespace corn {
    static void glfwErrorCallback(int error, const char *description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        auto* interfaceImpl = static_cast<Interface::InterfaceImpl*>(glfwGetWindowUserPointer(window));
        interfaceImpl->mouseScroll.x = static_cast<float>(xoffset);
        interfaceImpl->mouseScroll.y = static_cast<float>(yoffset);
    }

    static void glfwInputCallback(GLFWwindow* window, unsigned int codepoint) {
        auto* interfaceImpl = static_cast<Interface::InterfaceImpl*>(glfwGetWindowUserPointer(window));
        char utf8[5];
        size_t len = unicodeToUTF8(codepoint, utf8);
        if (len > 0) {
            interfaceImpl->input.append(utf8, len);
        }
    }

    static void setView(bgfx::ViewId viewID, const Vec<uint16_t, 2>& topLeft, const Vec<uint16_t, 2>& size) {
        bgfx::setViewRect(viewID, topLeft.x, topLeft.y, size.x, size.y);

        // Create an orthographic projection matrix mapping [0, fwidth] and [0, fheight] to clip space.
        float view[16];
        bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
        bx::Vec3 eye = { 0.0f, 0.0f, -1.0f };
        bx::mtxLookAt(view, eye, at);
        const bgfx::Caps* caps = bgfx::getCaps();
        float proj[16];
        bx::mtxOrtho(
                proj,
                0, static_cast<float>(size.x), static_cast<float>(size.y), 0,
                0.0f, 100.0f,
                0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewID, view, proj);
    }

    static void onWindowFramebufferResize(int fwidth, int fheight) {
        bgfx::reset((uint32_t)fwidth, (uint32_t)fheight, BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4);
        setView(0, Vec<uint16_t, 2>(0, 0), Vec<uint16_t, 2>(fwidth, fheight));
    }

    Interface::Interface(const Game& game)
            : game_(game), impl_(new Interface::InterfaceImpl()) {}

    Interface::~Interface() {
        delete this->impl_;
        bgfx::shutdown();
        glfwTerminate();
    }

    void Interface::init() {
        const Config& config = this->game_.getConfig();

        // Create a GLFW window without an OpenGL context.
        glfwSetErrorCallback(glfwErrorCallback);
        glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
        if (!glfwInit()) return;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

        // Create a GLFW window with the specified configuration.
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        switch (config.mode) {
            case DisplayMode::WINDOWED:
                glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
                this->impl_->window = glfwCreateWindow(
                        static_cast<int>(config.width), static_cast<int>(config.height),
                        config.title.c_str(),
                        nullptr, nullptr);
                break;
            case DisplayMode::WINDOWED_FIXED:
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                this->impl_->window = glfwCreateWindow(
                        static_cast<int>(config.width), static_cast<int>(config.height),
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

        // GLFW callbacks
        glfwSetWindowUserPointer(this->impl_->window, this->impl_);
        glfwSetScrollCallback(this->impl_->window, glfwScrollCallback);
        glfwSetCharCallback(this->impl_->window, glfwInputCallback);

        // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
        // Most graphics APIs must be used on the same thread that created the window.
        bgfx::renderFrame();

        // Initialize bgfx using the native window handle.
        bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        init.platformData.ndt = glfwGetX11Display();
	    init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(this->impl_->window);
#elif BX_PLATFORM_OSX
        init.platformData.nwh = glfwGetCocoaWindow(this->impl_->window);
        init.type = bgfx::RendererType::Metal;
#elif BX_PLATFORM_WINDOWS
        init.platformData.nwh = glfwGetWin32Window(this->impl_->window);
        init.type = bgfx::RendererType::Direct3D12;
#endif

        // Initialize bgfx with the window resolution.
        int width, height, fwidth, fheight;
        glfwGetWindowSize(this->impl_->window, &width, &height);
        glfwGetFramebufferSize(this->impl_->window, &fwidth, &fheight);
        this->impl_->width = static_cast<uint16_t>(width);
        this->impl_->height = static_cast<uint16_t>(height);
        this->impl_->fwidth = static_cast<uint16_t>(fwidth);
        this->impl_->fheight = static_cast<uint16_t>(fheight);

        init.resolution.width = this->impl_->fwidth;
        init.resolution.height = this->impl_->fheight;
        init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4;
        if (!bgfx::init(init)) {
            throw std::runtime_error("Failed to initialize bgfx.");
        }
        this->impl_->viewID = 0;
        bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
        onWindowFramebufferResize(this->impl_->fwidth, this->impl_->fheight);

        // Shaders
        this->impl_->polygonShader.loadEmbedded("vs_triangle", "fs_triangle");
        this->impl_->bitmapShader.loadEmbedded("vs_bitmap", "fs_bitmap");
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
        int newWidth, newHeight, newFWidth, newFHeight;
        glfwGetWindowSize(this->impl_->window, &newWidth, &newHeight);
        glfwGetFramebufferSize(this->impl_->window, &newFWidth, &newFHeight);
        this->impl_->width = static_cast<uint16_t>(newWidth);
        this->impl_->height = static_cast<uint16_t>(newHeight);
        if (this->impl_->fwidth != newFWidth || this->impl_->fheight != newFHeight) {
            this->impl_->fwidth = static_cast<uint16_t>(newFWidth);
            this->impl_->fheight = static_cast<uint16_t>(newFHeight);
            onWindowFramebufferResize(this->impl_->fwidth, this->impl_->fheight);
        }
    }

    void Interface::handleUserInput() {
        glfwPollEvents();

        // Exit event
        if (glfwWindowShouldClose(this->impl_->window)) {
            EventArgsInputExit eventArgs;
            EventManager::instance().emit(eventArgs);
            this->game_.getTopScene()->getEventManager().emit(eventArgs);
        }

        // Check if the window is focused
        if (!glfwGetWindowAttrib(this->impl_->window, GLFW_FOCUSED)) {
            // If not, ignore all input events
            return;
            // todo: emit a global event and a top-scene event
        }

        this->handleMouseInput();
        this->handleKeyboardInput();
    }

    void Interface::clear() {
        const auto [r, g, b, a] = this->game_.getConfig().background.getRGBA();
        bgfx::setViewClear(
                0,
                BGFX_CLEAR_COLOR,
                static_cast<uint32_t>(r) << 24
                | static_cast<uint32_t>(g) << 16
                | static_cast<uint32_t>(b) << 8
                | static_cast<uint32_t>(a));
        bgfx::touch(0);
    }

    void Interface::render(Scene* scene) {
        // Render Entities
        scene->getEntityManager().tidy();
        for (const CCamera* camera : scene->getEntityManager().getCameras()) {
            this->impl_->viewID++;
            renderCamera2D(scene, camera);
        }

        // Render UI widgets
        this->impl_->viewID++;
        this->renderUI(scene->getUIManager());
    }

    void Interface::renderDebugOverlay(size_t fps) {
        const FontFamily* fontFamily = FontManager::instance().getDefault();
        if (fontFamily == nullptr) return;

        // Set up the view
        this->impl_->viewID++;
        setView(this->impl_->viewID, Vec<uint16_t, 2>(0, 0), Vec<uint16_t, 2>(this->impl_->fwidth, this->impl_->fheight));

        // Render debug text
        RichText richText = RichText().addText("FPS: " + std::to_string(fps), TextStyle(fontFamily, 12, Color::WHITE()));
        this->impl_->debugText.setRichText(richText);
        drawDebug(
                this->impl_->viewID,
                this->impl_->debugText, 10, 6, this->getHiDPIScale(),
                this->impl_->polygonShader, this->impl_->bitmapShader);
    }

    void Interface::update() {
        bgfx::frame();
        this->impl_->viewID = 0;
    }

    bool Interface::isPressed(MouseButton mouseButton) const noexcept {
        return this->impl_->mousePressed.contains(mouseButton) && this->impl_->mousePressed.at(mouseButton);
    }

    bool Interface::isPressed(Key key) const noexcept {
        return this->impl_->keyPressed.contains(key) && this->impl_->keyPressed.at(key);
    }

    void Interface::handleMouseInput() {
        double x_, y_;
        glfwGetCursorPos(this->impl_->window, &x_, &y_);
        bool insideWindow = x_ >= 0.0 && y_ >= 0.0 && x_ <= this->impl_->width && y_ <= this->impl_->height;

        // Mouse inside window event
        if (insideWindow ^ this->impl_->mouseInside) {
            this->impl_->mouseInside = insideWindow;
            // todo: emit a global event and a top-scene event
        }

        if (!insideWindow) return;
        auto x = static_cast<float>(x_);
        auto y = static_cast<float>(y_);

        // Mouse move event
        if (this->impl_->mousePosition != Vec2f(x, y)) {
            this->impl_->mousePosition = Vec2f(x, y);

            // Emit event globally and to the top scene
            EventArgsMouseMove eventArgs(Vec2f(x, y));
            EventManager::instance().emit(eventArgs);
            this->game_.getTopScene()->getEventManager().emit(eventArgs);

            // Only emit the world event if not caught by UI
            if (!this->game_.getTopScene()->getUIManager().onHover(eventArgs)) {
                EventArgsWorldMouseMove worldEventArgs(Vec2f(x, y));
                this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
            }
        }

        // Mouse click event
        for (auto [glfwMouseButton, mouseButton] : GLFW_CORN_MOUSE_BUTTONS) {
            int glfwStatus = glfwGetMouseButton(this->impl_->window, glfwMouseButton);
            ButtonEvent buttonEvent;
            if (glfwStatus == GLFW_PRESS) {
                buttonEvent = ButtonEvent::DOWN;
            } else if (glfwStatus == GLFW_RELEASE) {
                buttonEvent = ButtonEvent::UP;
            } else {
                continue;
            }

            // Emit events if the button state has changed
            if (this->impl_->mousePressed[mouseButton] ^ (buttonEvent == ButtonEvent::DOWN)) {
                this->impl_->mousePressed[mouseButton] = (buttonEvent == ButtonEvent::DOWN);

                // Emit event globally and to the top scene
                EventArgsMouseButton eventArgs(mouseButton, buttonEvent, Vec2f(x, y));
                EventManager::instance().emit(eventArgs);
                this->game_.getTopScene()->getEventManager().emit(eventArgs);

                // Only emit the world event if not caught by UI
                if (!this->game_.getTopScene()->getUIManager().onClick(eventArgs)) {
                    EventArgsWorldMouseButton worldEventArgs(mouseButton, buttonEvent, Vec2f(x, y));
                    this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                }
            }
        }

        // Mouse scroll event
        if (this->impl_->mouseScroll != Vec2f::O()) {
            // Emit event globally and to the top scene
            EventArgsMouseScroll eventArgs(this->impl_->mouseScroll, Vec2f(x, y));
            EventManager::instance().emit(eventArgs);
            this->game_.getTopScene()->getEventManager().emit(eventArgs);

            // Only emit the world event if not caught by UI
            if (!this->game_.getTopScene()->getUIManager().onScroll(eventArgs)) {
                EventArgsWorldMouseScroll worldEventArgs(this->impl_->mouseScroll, Vec2f(x, y));
                this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
            }

            // Reset mouse scroll
            this->impl_->mouseScroll = Vec2f::O();
        }
    }

    void Interface::handleKeyboardInput() {
        // Modifiers
        uint8_t modifiers = getModifiers(this->impl_->window);

        // Keyboard input event
        for (auto [glfwKey, key] : GLFW_CORN_KEYS) {
            int glfwStatus = glfwGetKey(this->impl_->window, glfwKey);
            ButtonEvent buttonEvent;
            if (glfwStatus == GLFW_PRESS) {
                buttonEvent = ButtonEvent::DOWN;
            } else if (glfwStatus == GLFW_RELEASE) {
                buttonEvent = ButtonEvent::UP;
            } else {
                continue;
            }

            // Emit events if the button state has changed
            if (this->impl_->keyPressed[key] ^ (buttonEvent == ButtonEvent::DOWN)) {
                this->impl_->keyPressed[key] = (buttonEvent == ButtonEvent::DOWN);

                // Emit event globally and to the top scene
                EventArgsKeyboard eventArgs(key, buttonEvent, modifiers, this->impl_->mousePosition);
                EventManager::instance().emit(eventArgs);
                this->game_.getTopScene()->getEventManager().emit(eventArgs);

                // Only emit the world event if not caught by UI
                if (!this->game_.getTopScene()->getUIManager().onKeyboard(eventArgs)) {
                    EventArgsWorldKeyboard worldEventArgs(key, buttonEvent, modifiers, this->impl_->mousePosition);
                    this->game_.getTopScene()->getEventManager().emit(worldEventArgs);
                }
            }
        }

        // Text input event
        if (!this->impl_->input.empty()) {
            EventArgsTextEntered eventArgs(this->impl_->input);
            EventManager::instance().emit(eventArgs);
            this->game_.getTopScene()->getEventManager().emit(eventArgs);
            this->game_.getTopScene()->getUIManager().onTextEntered(eventArgs);
            this->impl_->input.clear();
        }
    }

    Transform2D Interface::getCameraTransformation(const CCamera* camera) const {
        // Calculate window size
        Vec2f windowSize = this->windowLogicalSize();
        Vec2f percentWindowSize = windowSize * 0.01f;

        // Calculate camera viewport and FOV
        Vec2f viewportSize(
                camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y));
        Vec2f fovSize = Vec2f(
                camera->fovW.calc(1.0f, viewportSize.x * 0.01f, viewportSize.y * 0.01f),
                camera->fovH.calc(1.0f, viewportSize.x * 0.01f, viewportSize.y * 0.01f)) * (1 / camera->scale);

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
        setView(this->impl_->viewID,
                Vec<uint16_t, 2>(x * hidpiScale, y * hidpiScale),
                Vec<uint16_t, 2>(w * hidpiScale, h * hidpiScale));
        const auto [r, g, b, a] = camera->background.getRGBA();  // NOLINT
        bgfx::setViewClear(
                this->impl_->viewID,
                BGFX_CLEAR_COLOR,
                static_cast<uint32_t>(r) << 24
                | static_cast<uint32_t>(g) << 16
                | static_cast<uint32_t>(b) << 8
                | static_cast<uint32_t>(a));
        bgfx::touch(this->impl_->viewID);
        bgfx::setViewMode(this->impl_->viewID, bgfx::ViewMode::Sequential);

        // Render entities
        for (Entity* entity: scene->getEntityManager().getActiveEntitiesWith<CTransform2D>()) {
            auto cTransform = entity->getComponent<CTransform2D>();

            // Sprite
            auto sprite = entity->getComponent<CSprite>();
            if (sprite && sprite->active && sprite->image && sprite->image->impl_) {
                draw(this->impl_->viewID, *cTransform, *sprite, worldToCameraTransform, this->impl_->bitmapShader);
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
                draw(this->impl_->viewID, *cTransform, *text, worldToCameraTransform, this->impl_->bitmapShader);
            }
        }
    }

    void Interface::renderUI(UIManager& uiManager) {
        setView(this->impl_->viewID, Vec<uint16_t, 2>(0, 0), Vec<uint16_t, 2>(this->impl_->fwidth, this->impl_->fheight));

        // Calculate window size
        Vec2f windowSize = this->windowLogicalSize();
        float hidpiScale = this->getHiDPIScale();

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

            // Update children viewport
            auto [vpul, vpbr] = subviewports[parent];
            auto scissorsX = static_cast<uint16_t>(std::round((vpul.x) * hidpiScale));
            auto scissorsY = static_cast<uint16_t>(std::round((vpul.y) * hidpiScale));
            auto scissorsW = static_cast<uint16_t>(std::round((vpbr.x - vpul.x) * hidpiScale));
            auto scissorsH = static_cast<uint16_t>(std::round((vpbr.y - vpul.y) * hidpiScale));
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

            // Set scissors
            bgfx::setScissor(scissorsX, scissorsY, scissorsW, scissorsH);

            // Render the background
            const auto [r, g, b, a] = widget->getBackground().getRGBA();
            TransientPolygonRenderer::draw(
                    this->impl_->viewID, this->impl_->polygonShader,
                    {
                        { x * hidpiScale, y * hidpiScale }, { (x + w) * hidpiScale, y * hidpiScale },
                        { (x + w) * hidpiScale, (y + h) * hidpiScale }, { x * hidpiScale, (y + h) * hidpiScale },
                    },
                    { 0, 1, 2, 0, 2, 3 },
                    Color::rgb(r, g, b, (unsigned char)((float)a * opacities[widget])),
                    Transform2D::I());

            // Set scissors again
            bgfx::setScissor(scissorsX, scissorsY, scissorsW, scissorsH);

            // Render the widget
            switch (widget->getType()) {
                case UIType::PANEL:
                    break;
                case UIType::LABEL: {
                    drawUI(
                            this->impl_->viewID,
                            *dynamic_cast<UILabel*>(widget), w,
                            Transform2D::dilate(Vec2f(hidpiScale, hidpiScale)) * Transform2D::translate(Vec2f(x, y)),
                            this->impl_->bitmapShader);
                    break;
                }
                case UIType::IMAGE: {// Scale image
                    drawUI(
                            this->impl_->viewID,
                            *dynamic_cast<UIImage*>(widget), w, h,
                            Transform2D::dilate(Vec2f(hidpiScale, hidpiScale)) * Transform2D::translate(Vec2f(x, y)),
                            this->impl_->bitmapShader);
                    break;
                }
            }
        }
    }
}
