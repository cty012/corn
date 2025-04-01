#pragma once

#include <corn/ui/ui_widget.h>

namespace corn {
    class Image;

    /**
     * @class UIImage
     * @brief Widget that displays image.
     *
     * @see UIWidget
     */
    class UIImage : public UIWidget {
    public:
        // UIManager need access to ctor/dtor
        friend class UIManager;

        [[nodiscard]] Vec2f getNaturalSize() const noexcept override;

        /// @brief Getter for the image.
        [[nodiscard]] const Image* getImage() const noexcept;

        /// @brief Setter for the image.
        void setImage(Image* image) noexcept;

    private:
        /**
         * @brief Constructor.
         * @param name Name of the UI widget.
         * @param image Pointer to an image object allocated on the heap. Memory managed by the UIImage class.
         */
        UIImage(WidgetID id, std::string name, UIManager& uiManager, Image* image) noexcept;

        /// @brief Destructor.
        ~UIImage() override;

        /// @brief The image to be rendered.
        Image* image_;
    };
}
