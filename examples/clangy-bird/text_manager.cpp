#include <fstream>
#include "text_manager.h"

TextManager& TextManager::instance() {
    static TextManager instance;
    return instance;
}

TextManager::TextManager() : text(), settings() {
    // Read texts
    std::ifstream iText("resources/text.json");
    std::ifstream iSettings("resources/settings.json");
    iText >> this->text;
    iSettings >> this->settings;
}

TextManager::~TextManager() = default;

std::string TextManager::get(const std::string& key) const {
    return this->text[key][this->settings["lang"]];
}

std::u8string TextManager::getu8(const std::string& key) const {
    std::string textLiteral = this->text[key][this->settings["lang"]];
    return {textLiteral.begin(), textLiteral.end()};
}

corn::RichText TextManager::getRichText(const std::string& key) const {
    corn::RichText result;
    for (const auto& item : this->text[key][this->settings["lang"]]) {
        std::string textLiteral = item["text"];
        const corn::Font* font = corn::FontManager::instance().get(item["font"][0]);
        int size = item["font"][1];
        corn::Color color = corn::Color::parse(item.value("color", "#ffffff"));
        corn::FontVariant variant = corn::FontVariant::REGULAR;
        std::string variantString = item.value("variant", "regular");
        if (variantString == "regular") {
            variant = corn::FontVariant::REGULAR;
        } else if (variantString == "bold") {
            variant = corn::FontVariant::BOLD;
        } else if (variantString == "italic") {
            variant = corn::FontVariant::ITALIC;
        } else if (variantString == "underline") {
            variant = corn::FontVariant::UNDERLINE;
        }
        result.addText(
                std::u8string(textLiteral.begin(), textLiteral.end()),
                corn::TextStyle(font, size, color, variant));
    }
    return result;
}
