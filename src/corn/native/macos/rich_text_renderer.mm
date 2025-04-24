#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include "macos/font_impl.h"
#include "macos/rich_text_renderer.h"

namespace corn {
    static CGAffineTransform getCGAffineTransform(const Transform2D& transform) {
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
        CFAttributedStringReplaceString(this->attrString_, CFRangeMake(0, CFAttributedStringGetLength(this->attrString_)), cfText);

        // Set the font attribute
        CFIndex start = 0;
        for (const RichText::Segment& segment : text.segments) {
            CFStringRef segmentText = CFStringCreateWithCString(kCFAllocatorDefault, segment.text.c_str(), kCFStringEncodingUTF8);
            CFIndex len = CFStringGetLength(segmentText);
            CFRange range = CFRangeMake(start, len);
            CFRelease(segmentText);

            // Set the font face and size
            float actualFontSize =
                    segment.style.position == FontPosition::REGULAR ?
                    segment.style.size :
                    segment.style.size * 0.66f;
            CTFontRef ctFont = segment.style.font->createCTFont(actualFontSize, segment.style.weight, segment.style.italic);
            CFAttributedStringSetAttribute(this->attrString_, range, kCTFontAttributeName, ctFont);
            CFRelease(ctFont);

            // Set the font color
            const auto [r, g, b, a] = segment.style.color.getRGBA();
            CGColorRef color = CGColorCreate(this->colorSpace_, (CGFloat[]) {
                    (CGFloat) r / 255.0f, (CGFloat) g / 255.0f, (CGFloat) b / 255.0f, (CGFloat) a / 255.0f,
            });
            CFAttributedStringSetAttribute(this->attrString_, range, kCTForegroundColorAttributeName, color);
            CFRelease(color);

            // Set the underline
            int32_t underlineNumber = segment.style.underline ? kCTUnderlineStyleSingle : kCTUnderlineStyleNone;
            CFNumberRef underlineNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &underlineNumber);
            CFAttributedStringSetAttribute(this->attrString_, range, kCTUnderlineStyleAttributeName, underlineNumberRef);
            CFRelease(underlineNumberRef);

            // Set the position
            int32_t positionNumber =
                    segment.style.position == FontPosition::SUPERSCRIPT ? 1 :
                    segment.style.position == FontPosition::SUBSCRIPT ? -1 :
                    0;
            CFNumberRef positionNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &positionNumber);
            CFAttributedStringSetAttribute(this->attrString_, range, kCTSuperscriptAttributeName, positionNumberRef);
            CFRelease(positionNumberRef);

            // Proceed to the next segment
            start += len;
        }

        // Framesetter
        this->framesetter_ = CTFramesetterCreateWithAttributedString(this->attrString_);

        // Find the natural size of the text.
        CGSize naturalSize = CTFramesetterSuggestFrameSizeWithConstraints(
                this->framesetter_, CFRangeMake(0, 0), nullptr, { CGFLOAT_MAX, CGFLOAT_MAX }, nullptr);
        this->naturalSize_.x = static_cast<float>(naturalSize.width);
        this->naturalSize_.y = static_cast<float>(naturalSize.height);

        // Release resources
        CFRelease(cfText);

        this->bitmapDirty_ = true;
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

        this->bitmapDirty_ = true;
    }

    const Vec2f& RichTextRenderer::getNaturalSize() const {
        return this->naturalSize_;
    }

    const Vec2f& RichTextRenderer::getSize() const {
        return this->size_;
    }

    void RichTextRenderer::setTransform(const Transform2D& transform) {
        // Check if we can reuse the old transform and bitmap
        if (!this->bitmapDirty_) {
            const Mat3f& oldMat = this->transform_.getMat();
            const Mat3f& newMat = transform.getMat();
            if (oldMat.to<3, 2>() == newMat.to<3, 2>() && oldMat[2][2] == newMat[2][2]) {
                float diff02 = newMat[0][2] - oldMat[0][2];
                float diff12 = newMat[1][2] - oldMat[1][2];
                float roundDiff02 = std::round(diff02);
                float roundDiff12 = std::round(diff12);
                if (std::abs(roundDiff02 - diff02) <= 0.0001f && std::abs(roundDiff12 - diff12) <= 0.0001f) {
                    // Integer translation, change the offset
                    this->offset_.x += roundDiff02;
                    this->offset_.y += roundDiff12;
                    this->transform_ = Transform2D::translate(Vec2f(roundDiff02, roundDiff12)) * this->transform_;
                    return;
                }
            }
        }

        // Update the transform
        this->transform_ = transform;

        // Update the bitmap
        uint16_t w, h;
        this->createBitmap(w, h);
        this->bitmapRenderer_.update(this->bitmapBuffer_.data(), this->offset_.x, this->offset_.y, w, h);
        this->bitmapDirty_ = false;
    }

    void RichTextRenderer::draw(bgfx::ViewId viewID, const Shader& shader) {
        this->bitmapRenderer_.draw(viewID, shader);
    }

    void RichTextRenderer::createBitmap(uint16_t& bitmapWidth, uint16_t& bitmapHeight) {
        if (!this->colorSpace_) {
            this->colorSpace_ = CGColorSpaceCreateDeviceRGB();
        }

        /// Determine the typographic bounds.
        // Apply transform to the text to find the actual width and height.
        Vec2f ul = this->transform_.mapPoint(Vec2f(0, this->size_.y));
        Vec2f ur = this->transform_.mapPoint(this->size_);
        Vec2f bl = this->transform_.mapPoint(Vec2f::O());
        Vec2f br = this->transform_.mapPoint(Vec2f(this->size_.x, 0));

        // Find the bounding box of the transformed points.
        auto minX = static_cast<int16_t>(std::floor(std::fmin(std::fmin(ul.x, ur.x), std::fmin(bl.x, br.x))));
        auto minY = static_cast<int16_t>(std::floor(std::fmin(std::fmin(ul.y, ur.y), std::fmin(bl.y, br.y))));
        auto maxX = static_cast<int16_t>(std::ceil(std::fmax(std::fmax(ul.x, ur.x), std::fmax(bl.x, br.x))));
        auto maxY = static_cast<int16_t>(std::ceil(std::fmax(std::fmax(ul.y, ur.y), std::fmax(bl.y, br.y))));
        bitmapWidth = maxX - minX;
        bitmapHeight = maxY - minY;
        this->offset_.x = minX;
        this->offset_.y = minY;

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

        CGAffineTransform cgTransform = getCGAffineTransform(this->transform_);
        CGAffineTransform contextCGTransform = CGAffineTransformConcat(
                CGAffineTransformMake(1, 0, 0, -1, 0, this->size_.y), cgTransform);

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
