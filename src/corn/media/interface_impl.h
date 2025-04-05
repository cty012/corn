#pragma once

#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../render/shader.h"

namespace corn {
    class Interface::InterfaceImpl {
    public:
        GLFWwindow* window;
        uint16_t width, height, fwidth, fheight;

        Shader polygonShader, bitmapShader;

        // Temp variable
        bgfx::ViewId viewID;

        InterfaceImpl();
    };
}
