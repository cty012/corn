#include <utility>
#include <corn/util/rich_text.h>

namespace corn {
    TextStyle::TextStyle(const FontFamily* fontFamily, float size, Color color) noexcept
            : fontFamily(fontFamily), size(size), color(std::move(color)),
              underline(false), position(FontPosition::REGULAR) {}

    TextStyle::TextStyle(
            const FontFamily* fontFamily, float size, Color color, const FontVariant& fontVariant,
            bool underline, FontPosition position) noexcept
            : fontFamily(fontFamily), size(size), color(std::move(color)), fontVariant(fontVariant),
              underline(underline), position(position) {}

    RichText::Segment::Segment(std::string text, TextStyle style) noexcept
            : text(std::move(text)), style(std::move(style)) {}

    RichText::RichText() noexcept : segments() {}

    RichText::~RichText() noexcept = default;

    RichText& RichText::addText(std::string text, TextStyle style) noexcept {
        this->segments.emplace_back(std::move(text), std::move(style));
        return *this;
    }

    std::string RichText::getString() const noexcept {
        std::string result;
        for (const Segment& segment : this->segments) {
            result += segment.text;
        }
        return result;
    }
}
