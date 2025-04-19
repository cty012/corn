#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
        file.clear();
        file.seekg(0, std::ios::beg);
        size_t chunkSize = 1024;  // Read in chunks of 1KB
        std::string chunk(chunkSize + 3, '\0');  // +3 to avoid "<svg" splitting across chunks
        size_t totalRead = 0;

        while (file && totalRead < 1024 * 1024) {  // Limit to 1MB
            file.read(chunk.data(), static_cast<long long>(chunkSize + 3));
            auto bytesRead = static_cast<size_t>(file.gcount());
            totalRead += bytesRead;
            // Create a buffer from the read chunk and convert to lowercase.
            std::string buffer = chunk.substr(0, bytesRead);
            std::ranges::transform(buffer, buffer.begin(),
                                   [](char c) { return static_cast<char>(std::tolower(c)); });
            if (buffer.find("<svg") != std::string::npos) {
                return ImageType::SVG;
            }

            // If we haven't reached EOF, rewind by 3 bytes to ensure tags split across chunks aren't missed.
            if (!file.eof()) {
                std::streampos pos = file.tellg();
                if (pos >= std::streampos(3)) {
                    file.seekg(-3, std::ios::cur);
                    totalRead -= 3;  // Adjust our counter since we're re-reading these bytes.
                }
            }
        }

        return ImageType::UNKNOWN;
    }

    ImageImpl::ImageImpl(const std::filesystem::path& path) : svgImage(), scale(1.0f, 1.0f) {
        this->type = detectImageType(path);

        switch (this->type) {
        case ImageType::PNG:
        case ImageType::JPEG: {
            std::string msg = "Failed to load image: " + path.string() + ".";

            // Load the image file into memory
            int width, height, channels;
            uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4); // RGBA
            if (!data) {
                throw ResourceLoadFailed("Failed to load image: " + path.string() + ".");
            }

            // Convert to BGRA8 format
            this->size.x = static_cast<uint32_t>(width);
            this->size.y = static_cast<uint32_t>(height);
            this->bitmapData.resize(width * height * 4);
            for (uint32_t i = 0; i < this->size.x * this->size.y; i++) {
                this->bitmapData[i * 4] = data[i * 4 + 2];
                this->bitmapData[i * 4 + 1] = data[i * 4 + 1];
                this->bitmapData[i * 4 + 2] = data[i * 4];
                this->bitmapData[i * 4 + 3] = data[i * 4 + 3];
            }

            // Free the image data
            stbi_image_free(data);
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
        case ImageType::BITMAP:
        case ImageType::UNKNOWN:
            throw ResourceLoadFailed("Unsupported image format for: " + path.string() + ".");
        }
    }

    ImageImpl::ImageImpl(uint32_t width, uint32_t height, const Color& color)
            : type(ImageType::BITMAP), svgImage(), scale(1.0f, 1.0f) {

        this->size.x = width;
        this->size.y = height;
        this->bitmapData.resize(width * height * 4);
        for (uint32_t i = 0; i < width * height; i++) {
            const auto [r, g, b, a] = color.getRGBA();
            this->bitmapData[i * 4] = b;
            this->bitmapData[i * 4 + 1] = g;
            this->bitmapData[i * 4 + 2] = r;
            this->bitmapData[i * 4 + 3] = a;
        }
    }

    ImageImpl::ImageImpl(uint32_t width, uint32_t height, const std::vector<Color>& bitmap)
            : type(ImageType::BITMAP), svgImage(), scale(1.0f, 1.0f) {

        this->size.x = width;
        this->size.y = height;
        this->bitmapData.resize(width * height * 4, 0);
        for (uint32_t i = 0; i < std::min(width * height, static_cast<uint32_t>(bitmap.size())); i++) {
            const auto [r, g, b, a] = bitmap[i].getRGBA();
            this->bitmapData[i * 4] = b;
            this->bitmapData[i * 4 + 1] = g;
            this->bitmapData[i * 4 + 2] = r;
            this->bitmapData[i * 4 + 3] = a;
        }
    }

    ImageImpl::~ImageImpl() {
        this->destroy();
    }

    ImageImpl::ImageImpl(const ImageImpl& other) {
        this->path = other.path;
        this->type = other.type;
        switch (this->type) {
            case ImageType::PNG:
            case ImageType::JPEG:
            case ImageType::BITMAP:
                this->bitmapData = other.bitmapData;
                this->bitmapRendererDirty_ = true;
                this->size = other.size;
                break;
            case ImageType::SVG:
                this->svgContent = other.svgContent;
                this->svgImage = createSVGImage(other.svgContent, "px", 96.0f);
            case ImageType::UNKNOWN:
                return;
        }
        this->scale = other.scale;
    }

    ImageImpl& ImageImpl::operator=(const ImageImpl& other) {
        if (this == &other) return *this;
        this->destroy();

        this->path = other.path;
        this->type = other.type;
        switch (this->type) {
            case ImageType::PNG:
            case ImageType::JPEG:
            case ImageType::BITMAP:
                this->bitmapData = other.bitmapData;
                this->bitmapRendererDirty_ = true;
                this->size = other.size;
                break;
            case ImageType::SVG:
                this->svgContent = other.svgContent;
                this->svgImage = createSVGImage(other.svgContent, "px", 96.0f);
            case ImageType::UNKNOWN:
                return *this;
        }
        this->scale = other.scale;

        return *this;
    }

    ImageImpl::ImageImpl(ImageImpl&& other) noexcept {
        this->path = std::move(other.path);
        this->type = other.type;
        this->bitmapData = std::move(other.bitmapData);
        this->bitmapRendererDirty_ = other.bitmapRendererDirty_;
        this->bitmapRenderer_ = std::move(other.bitmapRenderer_);
        this->size = other.size;
        this->svgContent = std::move(other.svgContent);
        this->svgImage = other.svgImage;
        this->scale = other.scale;

        other.type = ImageType::UNKNOWN;
        other.bitmapRendererDirty_ = true;
        other.svgImage = nullptr;
    }

    ImageImpl& ImageImpl::operator=(ImageImpl&& other) noexcept {
        if (this == &other) return *this;
        this->destroy();

        this->path = std::move(other.path);
        this->type = other.type;
        this->bitmapData = std::move(other.bitmapData);
        this->bitmapRenderer_ = std::move(other.bitmapRenderer_);
        this->size = other.size;
        this->svgContent = std::move(other.svgContent);
        this->svgImage = other.svgImage;
        this->scale = other.scale;

        other.type = ImageType::UNKNOWN;
        other.bitmapRendererDirty_ = true;
        other.svgImage = nullptr;

        return *this;
    }

    void ImageImpl::destroy() {
        this->bitmapRenderer_.destroy();
        nsvgDelete(this->svgImage);
        this->svgImage = nullptr;
    }

    float ImageImpl::getWidth() const {
        switch (this->type) {
            case ImageType::SVG:
                return this->svgImage->width;
            default:
                return static_cast<float>(this->size.x);
        }
    }

    float ImageImpl::getHeight() const {
        switch (this->type) {
            case ImageType::SVG:
                return this->svgImage->height;
            default:
                return static_cast<float>(this->size.y);
        }
    }

    bool ImageImpl::rasterize(Vec2f extraScale, bool useCache) {
        (void)extraScale; (void)useCache;
        return true;
        // todo
        // if (this->type != ImageType::SVG || this->svgImage == nullptr) {
        //     return false;
        // }
        //
        // auto actualWidth = static_cast<unsigned int>(std::ceil(this->svgImage->width * this->scale.x * extraScale.x));
        // auto actualHeight = static_cast<unsigned int>(std::ceil(this->svgImage->height * this->scale.y * extraScale.y));
        //
        // // Check if the image is already rasterized and cached
        // if (useCache && this->image.getSize().x == actualWidth && this->image.getSize().y == actualHeight) {
        //     return true; // Already rasterized and cached
        // }
        //
        // // Create a rasterizer
        // NSVGrasterizer* rast = nsvgCreateRasterizer();
        // if (!rast) {
        //     return false;
        // }
        //
        // std::vector<unsigned char> buffer(actualWidth * actualHeight * 4, 0);
        //
        // // Rasterize the SVG into the buffer
        // nsvgRasterizeXY(
        //     rast, this->svgImage, 0, 0,
        //     this->scale.x * extraScale.x, this->scale.y * extraScale.y,
        //     buffer.data(), (int)actualWidth, (int)actualHeight, (int)actualWidth * 4);
        //
        // // Load the buffer into an SFML image
        // this->image.create(actualWidth, actualHeight, buffer.data());
        //
        // // Load the rasterized image into the texture
        // if (!this->texture.loadFromImage(image)) {
        //     throw ResourceLoadFailed("Failed to load texture: " + path.string() + ".");
        // }
        // this->sfSprite = sf::Sprite(this->texture);
        //
        // nsvgDeleteRasterizer(rast);
        // return true;
    }

    BitmapRenderer& ImageImpl::getBitmapRenderer() {
        if (this->bitmapRendererDirty_) {
            this->bitmapRenderer_.update(
                    this->bitmapData.data(), 0.0f, 0.0f,
                    static_cast<uint16_t>(this->size.x), static_cast<uint16_t>(this->size.y));
            this->bitmapRendererDirty_ = false;
        }
        return this->bitmapRenderer_;
    }
}
