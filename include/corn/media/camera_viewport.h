#pragma once

#include <corn/util/expression.h>

namespace corn {
    class CameraViewportImpl;

    class CameraViewport {
    public:
        /// @brief Constructor.
        CameraViewport() noexcept;

        /// @brief Destructor.
        ~CameraViewport();

        CameraViewport(const CameraViewport& other) = delete;
        CameraViewport& operator=(const CameraViewport& other) = delete;

        /**
         * @brief Defines the viewport's top-left corner and the width and height.
         *
         * The viewport defines where the contents of the camera will be rendered on the window.
         * Units include pixels (px), percentage of window width (%ww), and percentage of window height (%wh).
         *
         * For detailed usage, see the documentation of @Expression.
         */
        Expression<3> x, y, w, h;

        /// @brief Pimpl idiom.
        CameraViewportImpl* impl_;
    };
}
