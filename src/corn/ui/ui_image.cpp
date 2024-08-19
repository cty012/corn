#include <corn/media/image.h>
#include <corn/ui/ui_image.h>

namespace corn {
    UIImage::UIImage(UIWidget::WidgetID id, std::string name, UIManager& uiManager, Image* image) noexcept
            : UIWidget(UIType::IMAGE, id, std::move(name), uiManager), image_(image) {}

    UIImage::~UIImage() {
        delete this->image_;
    }

    Vec2 UIImage::getNaturalSize() const noexcept {
        return this->image_->getSize();
    }

    const Image* UIImage::getImage() const noexcept {
        return this->image_;
    }

    void UIImage::setImage(Image* image) noexcept {
        delete this->image_;
        this->image_ = image;
    }
}
