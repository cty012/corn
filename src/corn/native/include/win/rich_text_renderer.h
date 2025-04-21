#pragma once

#include <cstdint>
#include <vector>
#include <bgfx/bgfx.h>
#include <corn/geometry/transform.h>
#include <corn/media/rich_text_frame.h>
#include <corn/util/rich_text.h>
#include "../../../render/bitmap_renderer.h"
#include "../../../render/shader.h"
#include "win/drawing_effect.h"
#include "win/font_family.h"

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
        void createBaseFormat();
        void createBitmap(uint16_t& bitmapWidth, uint16_t& bitmapHeight);

        // todo: destroyers
        void destroyBaseFormat();
        void destroyTextLayout();
        void destroyDrawingEffects();
        void destroyBitmapRenderer();

        // Constants
        IDWriteTextFormat* baseFormat_ = nullptr;

        // Changed by setRichText
        IDWriteTextLayout* textLayout_ = nullptr;
        std::vector<IDWriteFontCollection*> fontCollections_;  // Keep font collections alive
        std::vector<IDWriteFontFile*> fontFiles_;  // Keep font files alive
        std::vector<DrawingEffect*> drawingEffects_;
        Vec2f naturalSize_;

        // Changed by setFormat
        Vec2f size_;

        // Changed by setTransform
        Transform2D transform_;
        Vec2f offset_;
        std::vector<uint8_t> bitmapBuffer_;
        bool bitmapDirty_ = true;
        BitmapRenderer bitmapRenderer_;
    };
}
