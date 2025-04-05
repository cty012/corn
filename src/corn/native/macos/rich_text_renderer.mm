#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <corn/util/string_utils.h>
#include "macos/font_impl.h"
#include "macos/rich_text_renderer.h"

namespace corn {
    inline CGAffineTransform getCGAffineTransform(const Transform2D& transform) {
        const Mat3f& mat = transform.getMat();
        return {
            .a = mat[0][0],
            .b = mat[1][0],
            .c = mat[0][1],
            .d = mat[1][1],
            .tx = mat[0][2],
            .ty = mat[1][2],
        };
    }

    inline CGAffineTransform getCGAffineTransformRendering(const Transform2D& transform, uint16_t originalHeight) {
        (void) originalHeight;
        CGAffineTransform flip1 = CGAffineTransformMake(1, 0, 0, -1, 0, originalHeight);
        CGAffineTransform cgTransform = getCGAffineTransform(transform);
        return CGAffineTransformConcat(flip1, cgTransform);
    }

    RichTextRenderer::RichTextRenderer() = default;

    RichTextRenderer::~RichTextRenderer() {
        this->destroy();
    }

    void RichTextRenderer::destroy() {
        this->destroyBitmapRenderer();
        this->destroyFrame();
        this->destroyFramesetter();
        if (this->attrString_) {
            CFRelease(this->attrString_);
            this->attrString_ = nullptr;
        }
        if (this->colorSpace_) {
            CGColorSpaceRelease(this->colorSpace_);
            this->colorSpace_ = nullptr;
        }
    }

    void RichTextRenderer::setRichText(const RichText& text) {
        // Reset
        this->destroyFramesetter();
        if (!this->attrString_) {
            this->attrString_ = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
        }
        if (!this->colorSpace_) {
            this->colorSpace_ = CGColorSpaceCreateDeviceRGB();
        }

        // Create the attributed string
        CFStringRef cfText = CFStringCreateWithCString(kCFAllocatorDefault, text.getString().c_str(), kCFStringEncodingUTF8);
        CFAttributedStringReplaceString(this->attrString_, CFRangeMake(0, 0), cfText);

        // Set the font attribute
        CFIndex start = 0;
        for (const RichText::Segment& segment : text.segments) {
            auto len = static_cast<CFIndex>(count(segment.text));

            // Set the font family and size
            CTFontRef ctFont = CTFontCreateWithGraphicsFont(
                    segment.style.font->cgFont, segment.style.size, nullptr, nullptr);
            CFAttributedStringSetAttribute(
                    this->attrString_, CFRangeMake(start, start + len), kCTFontAttributeName, ctFont);
            CFRelease(ctFont);

            // Set the font color
            const auto [r, g, b, a] = segment.style.color.getRGBA();
            CGColorRef color = CGColorCreate(this->colorSpace_, (CGFloat[]) {
                    (CGFloat) r / 255.0f, (CGFloat) g / 255.0f, (CGFloat) b / 255.0f, (CGFloat) a / 255.0f,
            });
            CFAttributedStringSetAttribute(
                    this->attrString_, CFRangeMake(start, start + len), kCTForegroundColorAttributeName, color);
            CFRelease(color);

            // Set the font weight, slant, and decoration
            // todo

            start += len;
        }

        // Framesetter
        this->framesetter_ = CTFramesetterCreateWithAttributedString((CFAttributedStringRef) this->attrString_);

        // Find the natural size of the text.
        CGSize naturalSize = CTFramesetterSuggestFrameSizeWithConstraints(
                this->framesetter_, CFRangeMake(0, 0), nullptr, { CGFLOAT_MAX, CGFLOAT_MAX }, nullptr);
        this->naturalSize_.x = static_cast<float>(naturalSize.width);
        this->naturalSize_.y = static_cast<float>(naturalSize.height);

        // Release resources
        CFRelease(cfText);
    }

    void RichTextRenderer::setMaxWidth(float maxWidth) {
        this->destroyFrame();

        // Find the width and height of the text.
        this->maxWidth_ = maxWidth;
        if (this->maxWidth_ == 0.0f) {
            this->size_ = this->naturalSize_;
        } else {
            CGSize size = CTFramesetterSuggestFrameSizeWithConstraints(
                    this->framesetter_, CFRangeMake(0, 0), nullptr, { this->maxWidth_, CGFLOAT_MAX }, nullptr);
            this->size_.x = static_cast<float>(size.width);
            this->size_.y = static_cast<float>(size.height);
        }

        // Create the frame.
        CGPathRef path = CGPathCreateWithRect(CGRectMake(0, 0, this->size_.x, this->size_.y), nullptr);
        this->frame_ = CTFramesetterCreateFrame(this->framesetter_, CFRangeMake(0, 0), path, nullptr);

        // Release resources
        CGPathRelease(path);
    }

    const Vec2f& RichTextRenderer::getNaturalSize() const {
        return this->naturalSize_;
    }

    const Vec2f& RichTextRenderer::getSize() const {
        return this->size_;
    }

    void RichTextRenderer::setTransform(const Transform2D& transform) {
        // todo: compare the previous transform with the current one
        this->transform_ = transform;

        // Update the bitmap
        int16_t x, y;
        uint16_t w, h, bw, bh;
        this->getDim(x, y, w, h);
        this->createBitmap(bw, bh);
        this->bitmapRenderer_.update(this->bitmapBuffer_.data(), x, y, bw, bh);
    }

    void RichTextRenderer::draw(bgfx::ViewId viewID, const Shader& shader) {
        this->bitmapRenderer_.draw(viewID, shader);
    }

    void RichTextRenderer::getDim(int16_t& offsetX, int16_t& offsetY, uint16_t& bitmapWidth, uint16_t& bitmapHeight) {
        // Apply transform to the text to find the actual width and height.
        CGAffineTransform cgTransform = getCGAffineTransform(this->transform_);
        CGPoint ul = CGPointApplyAffineTransform(CGPointMake(0, 0), cgTransform);
        CGPoint ur = CGPointApplyAffineTransform(CGPointMake(this->size_.x, 0), cgTransform);
        CGPoint bl = CGPointApplyAffineTransform(CGPointMake(0, this->size_.y), cgTransform);
        CGPoint br = CGPointApplyAffineTransform(CGPointMake(this->size_.x, this->size_.y), cgTransform);

        auto minX = (int16_t) std::floor(std::fmin(std::fmin(ul.x, ur.x), std::fmin(bl.x, br.x)));
        auto minY = (int16_t) std::floor(std::fmin(std::fmin(ul.y, ur.y), std::fmin(bl.y, br.y)));
        auto maxX = (int16_t) std::ceil(std::fmax(std::fmax(ul.x, ur.x), std::fmax(bl.x, br.x)));
        auto maxY = (int16_t) std::ceil(std::fmax(std::fmax(ul.y, ur.y), std::fmax(bl.y, br.y)));
        bitmapWidth = maxX - minX;
        bitmapHeight = maxY - minY;
        offsetX = minX;
        offsetY = minY;
    }

    void RichTextRenderer::createBitmap(uint16_t& bitmapWidth, uint16_t& bitmapHeight) {
        if (!this->colorSpace_) {
            this->colorSpace_ = CGColorSpaceCreateDeviceRGB();
        }

        /// Determine the typographic bounds.
        // Apply transform to the text to find the actual width and height.
        CGAffineTransform cgTransform = getCGAffineTransform(this->transform_);
        CGPoint ul = CGPointApplyAffineTransform(CGPointMake(0, this->size_.y), cgTransform);
        CGPoint ur = CGPointApplyAffineTransform(CGPointMake(this->size_.x, this->size_.y), cgTransform);
        CGPoint bl = CGPointApplyAffineTransform(CGPointMake(0, 0), cgTransform);
        CGPoint br = CGPointApplyAffineTransform(CGPointMake(this->size_.x, 0), cgTransform);

        // Find the bounding box of the transformed points.
        auto minX = (int16_t) std::floor(std::fmin(std::fmin(ul.x, ur.x), std::fmin(bl.x, br.x)));
        auto minY = (int16_t) std::floor(std::fmin(std::fmin(ul.y, ur.y), std::fmin(bl.y, br.y)));
        auto maxX = (int16_t) std::ceil(std::fmax(std::fmax(ul.x, ur.x), std::fmax(bl.x, br.x)));
        auto maxY = (int16_t) std::ceil(std::fmax(std::fmax(ul.y, ur.y), std::fmax(bl.y, br.y)));
        bitmapWidth = maxX - minX;
        bitmapHeight = maxY - minY;

        /// Create the bitmap.
        // Allocate pixel data (BGRA8).
        size_t bitmapBufferSize = 4 * bitmapWidth * bitmapHeight;
        this->bitmapBuffer_.resize(bitmapBufferSize);
        std::memset(this->bitmapBuffer_.data(), 0, bitmapBufferSize * sizeof(uint8_t));

        // Create a bitmap context.
        CGContextRef context = CGBitmapContextCreate(
                this->bitmapBuffer_.data(), bitmapWidth, bitmapHeight, 8, bitmapWidth * 4,
                this->colorSpace_, (uint32_t) kCGImageAlphaPremultipliedFirst | (uint32_t) kCGBitmapByteOrder32Little);

        /// Apply transformation to the context.
        CGContextTranslateCTM(context, 0, bitmapHeight);
        CGContextScaleCTM(context, 1.0, -1.0);

        CGAffineTransform contextCGTransform = getCGAffineTransformRendering(this->transform_, this->size_.y);

        // Fit to bitmap
        contextCGTransform = CGAffineTransformConcat(contextCGTransform, CGAffineTransformMakeTranslation(-minX, -minY));
        CGContextConcatCTM(context, contextCGTransform);

        // Draw the text.
        CTFrameDraw(this->frame_, context);

        /// Clean up
        CGContextRelease(context);
    }

    void RichTextRenderer::destroyFramesetter() {
        if (this->framesetter_) {
            CFRelease(this->framesetter_);
            this->framesetter_ = nullptr;
        }
    }

    void RichTextRenderer::destroyFrame() {
        if (this->frame_) {
            CFRelease(this->frame_);
            this->frame_ = nullptr;
        }
    }

    void RichTextRenderer::destroyBitmapRenderer() {
        this->bitmapRenderer_.destroy();
    }
}
