#pragma once

#include <cstdint>
#include <vector>
#import <CoreText/CoreText.h>
#include <bgfx/bgfx.h>
#include <corn/geometry/transform.h>
#include <corn/media/rich_text_frame.h>
#include <corn/util/rich_text.h>
#include "../../../render/bitmap_renderer.h"
#include "../../../render/shader.h"
#include "macos/font_family.h"

namespace corn {
    class RichTextRenderer {
    public:
        RichTextRenderer();

        ~RichTextRenderer();

        void destroy();

        void setRichText(const RichText& richText);

        void setFormat(float maxWidth, WrapStyle wrapStyle, TextAlign textAlign);

        [[nodiscard]] const Vec2f& getNaturalSize() const;

        [[nodiscard]] const Vec2f& getSize() const;

        void setTransform(const Transform2D& transform);

        void draw(bgfx::ViewId viewID, const Shader& shader);

    private:
        void createBitmap(uint16_t& bitmapWidth, uint16_t& bitmapHeight);

        void destroyFramesetter();

        void destroyFrame();

        void destroyBitmapRenderer();

        // Constants
        CGColorSpaceRef colorSpace_ = nullptr;

        // Changed by setRichText
        CFMutableAttributedStringRef attrString_ = nullptr;
        CTFramesetterRef framesetter_ = nullptr;
        Vec2f naturalSize_;

        // Changed by setWidth
        float maxWidth_ = -1.0f;
        Vec2f size_;
        CTFrameRef frame_ = nullptr;

        // Changed by setTransform
        Transform2D transform_;
        Vec2f offset_;
        std::vector<uint8_t> bitmapBuffer_;
        bool bitmapDirty_ = true;
        BitmapRenderer bitmapRenderer_;
    };
}
