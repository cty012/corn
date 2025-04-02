#include <corn/media/text_render.h>
#include "../render/font_impl.h"
#include "../render/text_render_impl.h"

namespace corn {
    TextRender::TextRender(const RichText& richText)
            : impl_(new TextRenderImpl(richText)), linePadding_(0.0f), textAlign_(TextAlign::LEFT) {

        this->setWidthNoLimit();
    }

    TextRender::TextRender(const RichText& richText, float width, TextAlign textAlign)
            : impl_(new TextRenderImpl(richText)), linePadding_(0.0f), textAlign_(textAlign) {

        this->setWidth(width);
    }

    const RichText& TextRender::getText() const noexcept {
        return this->impl_->original;
    }

    void TextRender::setText(const RichText& richText) {
        bool limitWidth = this->impl_->limitWidth;
        float width = this->impl_->size.x;
        this->impl_->setRichText(richText, this->linePadding_);
        if (limitWidth) {
            this->setWidth(width);
        }
    }

    const Vec2f& TextRender::getSize() const noexcept {
        return this->impl_->size;
    }

    const Vec2f& TextRender::getNaturalSize() const noexcept {
        return this->impl_->naturalSize;
    }

    void TextRender::setWidth(float width) {
        width = std::max(width, 0.0f);
        if (this->impl_->limitWidth && this->impl_->size.x == width) return;
        this->impl_->setWidth(width, this->linePadding_);
    }

    void TextRender::setWidthNoLimit() {
        if (!this->impl_->limitWidth) return;
        this->impl_->setWidth(-1.0f, this->linePadding_);
    }

    float TextRender::getLinePadding() const noexcept {
        return this->linePadding_;
    }

    void TextRender::setLinePadding(float linePadding) {
        bool limitWidth = this->impl_->limitWidth;
        float width = this->impl_->size.x;
        this->linePadding_ = linePadding;
        if (limitWidth) {
            this->setWidth(width);
        } else {
            this->setWidth(-1.0f);
        }
    }

    TextAlign TextRender::getTextAlign() const noexcept {
        return this->textAlign_;
    }

    void TextRender::setTextAlign(TextAlign textAlign) {
        this->textAlign_ = textAlign;
    }
}
