#pragma once

#include <corn/media.h>
#include "nlohmann/json.hpp"

class TextManager {
public:
    static TextManager& instance();
    [[nodiscard]] std::string get(const std::string& key) const;
    [[nodiscard]] std::u8string getu8(const std::string& key) const;
    [[nodiscard]] corn::RichText getRichText(const std::string& key) const;

    void changeSettings(std::string key, nlohmann::json val) noexcept;
    void saveSettings() noexcept;

private:
    TextManager();
    ~TextManager();

    nlohmann::json text;
    nlohmann::json settings;
    std::string settingsPath;
};
