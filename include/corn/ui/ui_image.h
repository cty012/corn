#pragma once

#include <corn/media/image.h>
#include <corn/ui/ui_widget.h>

namespace corn {
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

        /// @brief Getter for the image.
        [[nodiscard]] const Image* getImage() const noexcept;

        /// @brief Setter for the image.
        void setImage(Image* image) noexcept;

    private:
        /// @brief Constructor.
        UIImage(WidgetID id, std::string name, UIManager& uiManager, Image* image) noexcept;

        /// @brief Destructor.
        ~UIImage() override;

        /// @brief The image to be rendered.
        Image* image_;
    };
}
