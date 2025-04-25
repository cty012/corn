#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include "macos/font_family.h"
#include "macos/rich_text_renderer.h"
#include "macos/utils.h"

namespace corn {
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
                    segment.style.size * 0.6f;
            CTFontRef ctFont = segment.style.fontFamily->createCTFont(actualFontSize, segment.style.fontVariant);
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
            float baselineShift =
                    segment.style.position == FontPosition::SUPERSCRIPT ? 0.4f * segment.style.size :
                    segment.style.position == FontPosition::SUBSCRIPT ? -0.15f * segment.style.size :
                    0.0f;
            CFNumberRef baselineShiftRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &baselineShift);
            CFAttributedStringSetAttribute(this->attrString_, range, kCTBaselineOffsetAttributeName, baselineShiftRef);
            CFRelease(baselineShiftRef);

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

    void RichTextRenderer::setFormat(float maxWidth, WrapStyle wrapStyle, TextAlign textAlign) {
        this->destroyFrame();
        this->destroyFramesetter();

        // Set the wrap style
        CTLineBreakMode lineBreakMode;
        switch (wrapStyle) {
            case WrapStyle::CHARACTER:
                lineBreakMode = kCTLineBreakByCharWrapping;
                break;
            case WrapStyle::WORD:
            case WrapStyle::NONE:
                lineBreakMode = kCTLineBreakByWordWrapping;
                break;
        }

        // Set the text alignment
        CTTextAlignment textAlignMode;
        switch (textAlign) {
            case TextAlign::LEFT:
                textAlignMode = kCTTextAlignmentLeft;
                break;
            case TextAlign::CENTER:
                textAlignMode = kCTTextAlignmentCenter;
                break;
            case TextAlign::RIGHT:
                textAlignMode = kCTTextAlignmentRight;
                break;
            case TextAlign::JUSTIFIED:
                textAlignMode = kCTTextAlignmentJustified;
                break;
        }

        // Set the paragraph style
        CTParagraphStyleRef paragraphStyle = CTParagraphStyleCreate(
                (const CTParagraphStyleSetting[]) {
                        { kCTParagraphStyleSpecifierLineBreakMode, sizeof(CTLineBreakMode), &lineBreakMode },
                        { kCTParagraphStyleSpecifierAlignment, sizeof(CTTextAlignment), &textAlignMode },
                }, 2);
        CFAttributedStringSetAttribute(
                this->attrString_, CFRangeMake(0, CFAttributedStringGetLength(this->attrString_)),
                kCTParagraphStyleAttributeName, paragraphStyle);
//        CFRelease(paragraphStyle);

        // Framesetter
        this->framesetter_ = CTFramesetterCreateWithAttributedString(this->attrString_);

        // Find the actual size of the text.
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
        Vec<int16_t, 4> bounds = calcBoundingBox(this->transform_, this->size_);
        this->offset_.x = bounds[0];
        this->offset_.y = bounds[1];
        bitmapWidth = bounds[2];
        bitmapHeight = bounds[3];

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
        contextCGTransform = CGAffineTransformConcat(
                contextCGTransform, CGAffineTransformMakeTranslation(-this->offset_.x, -this->offset_.y));
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
