#include <fstream>
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
            // if (!this->image.loadFromFile(path.string())) {
            //     throw ResourceLoadFailed("Failed to load image: " + path.string() + ".");
            // }
            //
            // // Load the image into the texture
            // if (!this->texture.loadFromImage(image)) {
            //     throw ResourceLoadFailed("Failed to load texture: " + path.string() + ".");
            // }
            // this->sfSprite = sf::Sprite(this->texture);
            // break;
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

    ImageImpl::ImageImpl(unsigned int width, unsigned int height, const Color& color)
            : type(ImageType::UNKNOWN), svgImage(), scale(1.0f, 1.0f) {

        (void)width; (void)height; (void)color;

        std::string msg = "Failed to create image.";
//        const auto [r, g, b, a] = color.getRGBA();  // NOLINT
        // this->image.create(width, height, sf::Color{ r, g, b, a });
        // if (!this->texture.loadFromImage(image)) throw ResourceLoadFailed(msg);
        // this->sfSprite = sf::Sprite(this->texture);
    }

    ImageImpl::~ImageImpl() {
        nsvgDelete(this->svgImage);
    }

    ImageImpl::ImageImpl(const ImageImpl& other) {
        this->path = other.path;
        this->type = other.type;
        // this->image = other.image;
        this->svgContent = other.svgContent;
        this->svgImage = createSVGImage(other.svgContent, "px", 96.0f);
        this->scale = other.scale;
        // this->texture = other.texture;
        // this->sfSprite = other.sfSprite;
    }

    ImageImpl& ImageImpl::operator=(const ImageImpl& other) {
        if (this == &other) return *this;

        this->path = other.path;
        this->type = other.type;
        // this->image = other.image;
        this->svgContent = other.svgContent;
        delete this->svgImage;
        this->svgImage = createSVGImage(other.svgContent, "px", 96.0f);
        this->scale = other.scale;
        // this->texture = other.texture;
        // this->sfSprite = other.sfSprite;

        return *this;
    }

    float ImageImpl::getWidth() const {
        switch (this->type) {
            case ImageType::SVG:
                return this->svgImage->width;
            default:
                // return static_cast<float>(this->image.getSize().x);
                return 0.0f;
        }
    }

    float ImageImpl::getHeight() const {
        switch (this->type) {
            case ImageType::SVG:
                return this->svgImage->height;
            default:
                // return static_cast<float>(this->image.getSize().y);
                return 0.0f;
        }
    }

    bool ImageImpl::rasterize(Vec2f extraScale, bool useCache) {
        (void)extraScale; (void)useCache;
        return true;
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
}
