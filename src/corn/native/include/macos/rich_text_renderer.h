#pragma once

#include <cstdint>
#import <CoreText/CoreText.h>
#include <bgfx/bgfx.h>
#include <corn/geometry/transform.h>
#include <corn/util/rich_text.h>
#include "../../../render/bitmap_renderer.h"
#include "../../../render/shader.h"
#include "font_impl.h"

namespace corn {
    class RichTextRenderer {
    public:
        RichTextRenderer();

        ~RichTextRenderer();

        void destroy();

        void setRichText(const RichText& richText);

        void setMaxWidth(float maxWidth);

        [[nodiscard]] const Vec2f& getNaturalSize() const;

        [[nodiscard]] const Vec2f& getSize() const;

        void setTransform(const Transform2D& transform);

        void draw(bgfx::ViewId viewID, const Shader& shader);

    private:
        void getDim(int16_t& x, int16_t& y, uint16_t& w, uint16_t& h);

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

        std::vector <uint8_t> bitmapBuffer_;

        StaticBitmapRenderer bitmapRenderer_;
    };
}
