#pragma once

#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>
#include "../render/shader.h"

namespace corn {
    class Interface::InterfaceImpl {
    public:
        GLFWwindow* window;
        bgfx::ViewId viewID;
        int width, height, fwidth, fheight;

        Shader polygonShader;

        InterfaceImpl();
    };
}
