#include <array>
#include <fstream>
#include <nanosvgrast.h>
#include <ranges>
#include <corn/media/image.h>
#include <corn/util/color.h>
#include <corn/util/exceptions.h>
#include "image_impl.h"
#include "nsvg_helper.h"

namespace corn {
    ImageType detectImageType(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return ImageType::UNKNOWN;

        std::array<unsigned char, 8> header{};
        file.read(reinterpret_cast<char*>(header.data()), header.size());

        // PNG Signature: 89 50 4E 47 0D 0A 1A 0A
        if (header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4E &&
            header[3] == 0x47 && header[4] == 0x0D && header[5] == 0x0A &&
            header[6] == 0x1A && header[7] == 0x0A) {
            return ImageType::PNG;
            }

        // JPEG Signature: FF D8 FF
        if (header[0] == 0xFF && header[1] == 0xD8 && header[2] == 0xFF) {
            return ImageType::JPEG;
        }

        // SVG Check: Look for '<svg' in the first 1024 bytes
        file.seekg(0);
        std::array<char, 1024> buffer{};
        file.read(buffer.data(), buffer.size());
        std::string content(buffer.data(), file.gcount());
        std::ranges::transform(content, content.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        if (content.find("<svg") != std::string::npos) {
            return ImageType::SVG;
        }

        return ImageType::UNKNOWN;
    }

    ImageImpl::ImageImpl(const std::filesystem::path& path) : svgImage(), scale(1.0f, 1.0f) {
        this->type = detectImageType(path);

        switch (this->type) {
        case ImageType::PNG:
        case ImageType::JPEG: {
            std::string msg = "Failed to load image: " + path.string() + ".";
            if (!this->image.loadFromFile(path.string())) {
                throw ResourceLoadFailed("Failed to load image: " + path.string() + ".");
            }

            // Load the image into the texture
            if (!this->texture.loadFromImage(image)) {
                throw ResourceLoadFailed("Failed to load texture: " + path.string() + ".");
            }
            this->sfSprite = sf::Sprite(this->texture);
            break;
        }
        case ImageType::SVG: {
            std::string msg = "Failed to parse SVG image: " + path.string() + ".";
            // Read the SVG file content
            if (!readFromSVGFile(path, this->svgContent)) {
                throw ResourceLoadFailed("Failed to load SVG image: " + path.string() + ".");
            }
            this->svgImage = createSVGImage(this->svgContent, "px", 96.0f);
            if (!this->svgImage) throw ResourceLoadFailed(msg);

            // Rasterize the SVG image
            if (!this->rasterize()) {
                throw ResourceLoadFailed("Failed to rasterize SVG image: " + path.string() + ".");
            }

            break;
        }
        case ImageType::UNKNOWN:
            throw ResourceLoadFailed("Unsupported image format for: " + path.string() + ".");
        }
    }

    ImageImpl::ImageImpl(unsigned int width, unsigned int height, Color color)
            : type(ImageType::UNKNOWN), svgImage(), scale(1.0f, 1.0f) {

        std::string msg = "Failed to create image.";
        auto [r, g, b, a] = color.getRGBA();
        this->image.create(width, height, sf::Color(r, g, b, a));
        if (!this->texture.loadFromImage(image)) throw ResourceLoadFailed(msg);
        this->sfSprite = sf::Sprite(this->texture);
    }

    ImageImpl::~ImageImpl() {
        nsvgDelete(this->svgImage);
    }

    ImageImpl::ImageImpl(const ImageImpl& other) {
        this->path = other.path;
        this->type = other.type;
        this->image = other.image;
        this->svgContent = other.svgContent;
        this->svgImage = createSVGImage(other.svgContent, "px", 96.0f);
        this->scale = other.scale;
        this->texture = other.texture;
        this->sfSprite = other.sfSprite;
    }

    ImageImpl& ImageImpl::operator=(const ImageImpl& other) {
        if (this == &other) return *this;

        this->path = other.path;
        this->type = other.type;
        this->image = other.image;
        this->svgContent = other.svgContent;
        delete this->svgImage;
        this->svgImage = createSVGImage(other.svgContent, "px", 96.0f);
        this->scale = other.scale;
        this->texture = other.texture;
        this->sfSprite = other.sfSprite;

        return *this;
    }

    float ImageImpl::getWidth() const {
        switch (this->type) {
            case ImageType::SVG:
                return this->svgImage->width;
            default:
                return static_cast<float>(this->image.getSize().x);
        }
    }

    float ImageImpl::getHeight() const {
        switch (this->type) {
            case ImageType::SVG:
                return this->svgImage->height;
            default:
                return static_cast<float>(this->image.getSize().y);
        }
    }

    bool ImageImpl::rasterize(Vec2 extraScale, bool useCache) {
        if (this->type != ImageType::SVG || this->svgImage == nullptr) {
            return false;
        }

        unsigned int actualWidth = static_cast<unsigned int>(std::ceil(this->svgImage->width * this->scale.x * extraScale.x));
        unsigned int actualHeight = static_cast<unsigned int>(std::ceil(this->svgImage->height * this->scale.y * extraScale.y));

        // Check if the image is already rasterized and cached
        if (useCache && this->image.getSize().x == actualWidth && this->image.getSize().y == actualHeight) {
            return true; // Already rasterized and cached
        }

        // Create a rasterizer
        NSVGrasterizer* rast = nsvgCreateRasterizer();
        if (!rast) {
            return false;
        }

        std::vector<unsigned char> buffer(actualWidth * actualHeight * 4, 0);

        // Rasterize the SVG into the buffer
        nsvgRasterizeXY(
            rast, this->svgImage, 0, 0,
            this->scale.x * extraScale.x, this->scale.y * extraScale.y,
            buffer.data(), actualWidth, actualHeight, actualWidth * 4);

        // Load the buffer into an SFML image
        this->image.create(actualWidth, actualHeight, buffer.data());

        // Load the rasterized image into the texture
        if (!this->texture.loadFromImage(image)) {
            throw ResourceLoadFailed("Failed to load texture: " + path.string() + ".");
        }
        this->sfSprite = sf::Sprite(this->texture);

        nsvgDeleteRasterizer(rast);
        return true;
    }

    Image::Image(const std::filesystem::path& path) {
        this->impl_ = new ImageImpl(path);
    }

    Image::Image(unsigned int width, unsigned int height, Color color) {
        this->impl_ = new ImageImpl(width, height, color);
    }

    Image::~Image() {
        delete this->impl_;
    }

    Image::Image(const Image& other) {
        this->impl_ = new ImageImpl(*other.impl_);
    }

    Image& Image::operator=(const Image& other) {
        if (&other == this) return *this;
        delete this->impl_;
        this->impl_ = new ImageImpl(*other.impl_);
        return *this;
    }

    Image::Image(Image&& other) noexcept {
        this->impl_ = other.impl_;
        other.impl_ = nullptr;
    }

    Image& Image::operator=(Image&& other) noexcept {
        if (&other == this) return *this;
        delete this->impl_;
        this->impl_ = other.impl_;
        other.impl_ = nullptr;
        return *this;
    }

    std::pair<unsigned int, unsigned int> Image::getOriginalSize() const noexcept {
        return { static_cast<unsigned int>(this->impl_->getWidth()), static_cast<unsigned int>(this->impl_->getHeight()) };
    }

    Vec2 Image::getSize() const noexcept {
        switch (this->impl_->type) {
            case ImageType::SVG:
                return Vec2(this->impl_->getWidth(), this->impl_->getHeight()) * this->impl_->scale.x;
            default:
                return Vec2(this->impl_->getWidth(), this->impl_->getHeight()) * this->impl_->scale;
        }
    }

    void Image::resize(float width, float height) {
        if (this->impl_ == nullptr) return;
        this->impl_->scale = Vec2(width / this->impl_->getWidth(), height / this->impl_->getHeight());
        if (this->impl_->type == ImageType::SVG) {
            this->impl_->rasterize();
        }
    }
}
