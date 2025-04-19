#include "win/font_collection_loader.h"
#include "win/rich_text_renderer.h"
#include "win/utils.h"

#include <comdef.h>

namespace corn {
    RichTextRenderer::RichTextRenderer() = default;

    RichTextRenderer::~RichTextRenderer() {
        this->destroy();
    }

    void RichTextRenderer::destroy() {
        // todo
    }

    void RichTextRenderer::setRichText(const RichText& richText) {
        this->createBaseFormat();
        this->destroyTextLayout();
        this->destroyDrawingEffects();
        IDWriteFactory5* factory = getDWriteFactory5();

        // Create a text layout
        std::wstring text = utf8To16(richText.getString());
        factory->CreateTextLayout(
                text.c_str(), static_cast<UINT32>(text.length()),
                this->baseFormat_,
                FLT_MAX, FLT_MAX,
                &this->textLayout_);

        // Apply styles
        UINT32 start = 0;
        for (const RichText::Segment& segment : richText.segments) {
            std::wstring segText = utf8To16(segment.text);
            auto len = static_cast<UINT32>(segText.length());
            DWRITE_TEXT_RANGE range = { start, len };

            // Set the font face
            const FontFamily* fontFamily = segment.style.fontFamily;
            if (fontFamily->fontFaces.contains(segment.style.fontVariant)) {
                // Option 1: Use custom font face
                const FontFace& fontFace = fontFamily->fontFaces.at(segment.style.fontVariant);

                // Keep the font collection alive
                fontFace.fontCollection->AddRef();
                this->fontCollections_.push_back(fontFace.fontCollection);

                // Keep the font file alive
                fontFace.fontFile->AddRef();
                this->fontFiles_.push_back(fontFace.fontFile);

                // Set the font face
                this->textLayout_->SetFontCollection(fontFace.fontCollection, range);
                this->textLayout_->SetFontFamilyName(fontFace.name.c_str(), range);
            } else if (!fontFamily->fontFamilyName.empty()) {
                // Option 2: Use system font face
                // No need to change the font collection, as it is the default
                this->textLayout_->SetFontFamilyName(fontFamily->fontFamilyName.c_str(), range);
            } else {
                // Option 3: Use default font face with synthesized variant
                const FontFace& fontFace = fontFamily->fontFaces.at(FontVariant());

                // Keep the font collection alive
                fontFace.fontCollection->AddRef();
                this->fontCollections_.push_back(fontFace.fontCollection);

                // Keep the font file alive
                fontFace.fontFile->AddRef();
                this->fontFiles_.push_back(fontFace.fontFile);

                // Set the font face
                this->textLayout_->SetFontCollection(fontFace.fontCollection, range);
                this->textLayout_->SetFontFamilyName(fontFace.name.c_str(), range);
            }

            // Set the font size
            float actualFontSize =
                    segment.style.position == FontPosition::REGULAR ?
                    segment.style.size :
                    segment.style.size * 0.5f;
            this->textLayout_->SetFontSize(actualFontSize, range);

            // Set the underline
            this->textLayout_->SetUnderline(segment.style.underline, range);

            // Set the font color and position
            auto drawingEffect = new DrawingEffect(segment.style.color, segment.style.position, segment.style.size);
            this->drawingEffects_.push_back(drawingEffect);
            this->textLayout_->SetDrawingEffect(drawingEffect, range);

            start += len;
        }

        // Find the natural size
        DWRITE_TEXT_METRICS metrics{};
        this->textLayout_->GetMetrics(&metrics);
        this->naturalSize_.x = metrics.width;
        this->naturalSize_.y = metrics.height;

        this->bitmapDirty_ = true;
    }

    void RichTextRenderer::setFormat(float maxWidth, WrapStyle wrapStyle, TextAlign textAlign) {
        // Max width
        if (maxWidth <= 0.0f) {
            maxWidth = FLT_MAX;
        }
        this->textLayout_->SetMaxWidth(maxWidth);

        // Word wrapping
        switch (wrapStyle) {
            case WrapStyle::WORD:
                this->textLayout_->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
                break;
            case WrapStyle::CHARACTER:
                this->textLayout_->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
                break;
            case WrapStyle::NONE:
                this->textLayout_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                break;
        }

        // Text align
        switch (textAlign) {
            case TextAlign::LEFT:
                this->textLayout_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                break;
            case TextAlign::RIGHT:
                this->textLayout_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                break;
            case TextAlign::CENTER:
                this->textLayout_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                break;
            case TextAlign::JUSTIFIED:
                this->textLayout_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
                break;
        }

        // Find the size
        DWRITE_TEXT_METRICS metrics{};
        this->textLayout_->GetMetrics(&metrics);
        this->size_.x = metrics.width;
        this->size_.y = metrics.height;

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
                }
                return;
            }
        }

        // Update the transform
        this->transform_ = transform;

        // Update the bitmap
        // todo
        uint16_t w, h;
        this->createBitmap(w, h);
        this->bitmapRenderer_.update(this->bitmapBuffer_.data(), this->offset_.x, this->offset_.y, w, h);
        this->bitmapDirty_ = false;
    }

    void RichTextRenderer::createBaseFormat() {
        if (!this->baseFormat_) {
            IDWriteFactory5* factory = getDWriteFactory5();
            factory->CreateTextFormat(
                L"Segoe UI", nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                12.0f, L"en-us",
                &this->baseFormat_
            );
        }
    }

    void RichTextRenderer::draw(bgfx::ViewId viewID, const Shader& shader) {
        this->bitmapRenderer_.draw(viewID, shader);
    }

    void RichTextRenderer::createRenderTarget() {
        ID2D1Factory* d2d1Factory = getD2D1Factory();

        // Create the bitmap
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                0.0f, 0.0f);     // use default DPI
        d2d1Factory->CreateWicBitmapRenderTarget(
                this->wicBitmap_,
                rtProps,
                &this->renderTarget_);

        // Release resources
        d2d1Factory->Release();
    }

    void RichTextRenderer::createBitmap(uint16_t& bitmapWidth, uint16_t& bitmapHeight) {
        this->destroyRenderTarget();
        this->destroyBitmap();
        IWICImagingFactory* wicFactory = getWICFactory();

        // todo: calculate the bitmap size
        this->offset_.x = this->transform_.getMat()[0][2];
        this->offset_.y = this->transform_.getMat()[1][2];
        bitmapWidth = static_cast<uint16_t>(ceil(this->size_.x));
        bitmapHeight = static_cast<uint16_t>(ceil(this->size_.y));

        // Create the bitmap
        wicFactory->CreateBitmap(
                bitmapWidth, bitmapHeight,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapCacheOnDemand,
                &this->wicBitmap_);
        wicFactory->Release();
        this->createRenderTarget();

        // Begin drawing
        this->renderTarget_->BeginDraw();
        this->renderTarget_->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

        // Draw the text layout
        // todo: use custom renderer
        D2D1_POINT_2F origin = D2D1::Point2F(0.0f, 0.0f);
        ID2D1SolidColorBrush* textBrush;
        this->renderTarget_->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f)),
                &textBrush);
        this->renderTarget_->DrawTextLayout(
                origin,
                this->textLayout_,
                textBrush,
                D2D1_DRAW_TEXT_OPTIONS_NONE);

        // End drawing
        this->renderTarget_->EndDraw();

        // Retrieve the bitmap data
        IWICBitmapLock* lock;
        WICRect rect = { 0, 0, bitmapWidth, bitmapHeight };
        this->wicBitmap_->Lock(&rect, WICBitmapLockRead, &lock);
        UINT bufferSize = 0;
        BYTE* pixels = nullptr;
        lock->GetDataPointer(&bufferSize, &pixels);
        this->bitmapBuffer_.resize(bufferSize);
        std::memcpy(this->bitmapBuffer_.data(), pixels, bufferSize);
        lock->Release();

        // Un-premultiply alpha
        for (size_t i = 0; i + 3 < this->bitmapBuffer_.size(); i += 4) {
            BYTE* pixel = &this->bitmapBuffer_[i];
            if (pixel[3] != 0) {
                auto r = static_cast<float>(pixel[0]);
                auto g = static_cast<float>(pixel[1]);
                auto b = static_cast<float>(pixel[2]);
                float a = static_cast<float>(pixel[3]) / 255.0f;
                pixel[0] = static_cast<uint8_t>(std::round(r / a));
                pixel[1] = static_cast<uint8_t>(std::round(g / a));
                pixel[2] = static_cast<uint8_t>(std::round(b / a));
            }
        }
    }

    void RichTextRenderer::destroyBaseFormat() {
        if (this->baseFormat_) {
            this->baseFormat_->Release();
            this->baseFormat_ = nullptr;
        }
    }

    void RichTextRenderer::destroyTextLayout() {
        if (this->textLayout_) {
            this->textLayout_->Release();
            this->textLayout_ = nullptr;
        }

        for (IDWriteFontCollection* fontCollection : this->fontCollections_) {
            fontCollection->Release();
        }
        this->fontCollections_.clear();

        for (IDWriteFontFile* fontFile : this->fontFiles_) {
            fontFile->Release();
        }
        this->fontFiles_.clear();
    }

    void RichTextRenderer::destroyDrawingEffects() {
        for (DrawingEffect* drawingEffect : this->drawingEffects_) {
            delete drawingEffect;
        }
        this->drawingEffects_.clear();
    }

    void RichTextRenderer::destroyBitmap() {
        if (this->wicBitmap_) {
            this->wicBitmap_->Release();
            this->wicBitmap_ = nullptr;
        }
    }

    void RichTextRenderer::destroyRenderTarget() {
        if (this->renderTarget_) {
            this->renderTarget_->Release();
            this->renderTarget_ = nullptr;
        }
    }

    void RichTextRenderer::destroyBitmapRenderer() {
        this->bitmapRenderer_.destroy();
    }
}
