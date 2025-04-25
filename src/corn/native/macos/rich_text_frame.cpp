#include <corn/media/rich_text_frame.h>
#include "macos/rich_text_renderer.h"

namespace corn {
    RichTextFrame::RichTextFrame(const RichText& richText) noexcept
            : richText_(richText), maxWidth_(0.0f), wrapStyle_(WrapStyle::WORD), textAlign_(TextAlign::LEFT),
              naturalSize_(0.0f, 0.0f), naturalSizeDirty_(true),
              size_(0.0f, 0.0f), sizeDirty_(true),
              richTextRenderer_(new RichTextRenderer()),
              richTextRendererRichTextDirty_(true), richTextRendererFormatDirty_(true) {}

    RichTextFrame::~RichTextFrame() noexcept {
        delete this->richTextRenderer_;
    }

    const RichText& RichTextFrame::getRichText() const noexcept {
        return this->richText_;
    }

    void RichTextFrame::setRichText(const RichText& richText) noexcept {
        if (this->richText_ == richText) {
            return;
        }

        this->richText_ = richText;
        this->naturalSizeDirty_ = true;
        this->sizeDirty_ = true;
        this->richTextRendererRichTextDirty_ = true;
    }

    float RichTextFrame::getMaxWidth() const noexcept {
        return this->maxWidth_;
    }

    void RichTextFrame::setMaxWidth(float maxWidth) noexcept {
        if (maxWidth < 0.0f) {
            maxWidth = 0.0f;
        }
        maxWidth = std::ceil(maxWidth);
        if (this->maxWidth_ == maxWidth) {
            return;
        }

        this->maxWidth_ = maxWidth;
        this->sizeDirty_ = true;
        this->richTextRendererFormatDirty_ = true;
    }

    WrapStyle RichTextFrame::getWrapStyle() const noexcept {
        return this->wrapStyle_;
    }

    void RichTextFrame::setWrapStyle(WrapStyle wrapStyle) noexcept {
        if (this->wrapStyle_ == wrapStyle) {
            return;
        }

        this->wrapStyle_ = wrapStyle;
        this->sizeDirty_ = true;
        this->richTextRendererFormatDirty_ = true;
    }

    TextAlign RichTextFrame::getTextAlign() const noexcept {
        return this->textAlign_;
    }



    const Vec2f& RichTextFrame::getNaturalSize() const {
        if (this->naturalSizeDirty_) {
            if (this->richTextRendererRichTextDirty_) {
                this->richTextRenderer_->setRichText(this->richText_);
                this->richTextRendererRichTextDirty_ = false;
                this->richTextRendererFormatDirty_ = true;
            }

            this->naturalSize_ = this->richTextRenderer_->getNaturalSize();
            this->naturalSizeDirty_ = false;
        }

        return this->naturalSize_;
    }

    const Vec2f& RichTextFrame::getSize() const {
        if (this->sizeDirty_) {
            (void)this->getRichTextRenderer();  // Ensure richTextRenderer is not dirty

            this->size_ = this->richTextRenderer_->getSize();
            this->sizeDirty_ = false;
        }

        return this->size_;
    }

    RichTextRenderer* RichTextFrame::getRichTextRenderer() const {
        if (this->richTextRendererRichTextDirty_) {
            this->richTextRenderer_->setRichText(this->richText_);
            this->richTextRendererRichTextDirty_ = false;
            this->richTextRendererFormatDirty_ = true;
        }

        if (this->richTextRendererFormatDirty_) {
            this->richTextRenderer_->setFormat(this->maxWidth_, this->wrapStyle_, this->textAlign_);
            this->richTextRendererFormatDirty_ = false;
        }

        return this->richTextRenderer_;
    }
}
