#include <cstdlib>
#include <fstream>
#include <utility>
#include "text_manager.h"

TextManager& TextManager::instance() {
    static TextManager instance;
    return instance;
}

std::string getSettingsRootFolder() {
    #if defined(_WIN32) || defined(_WIN64)
        // Windows
        #if defined(_MSC_VER)
            char* appDataPath = nullptr;
            size_t size = 0;
            if (_dupenv_s(&appDataPath, &size, "APPDATA") == 0 && appDataPath != nullptr) {
                std::string folderPath(appDataPath);
                free(appDataPath);  // Free the allocated memory
                return folderPath + R"(\corn-games\clangy-bird\settings\)";
            } else {
                throw std::runtime_error("APPDATA environment variable not found");
            }
        #else
            const char* appDataPath = std::getenv("APPDATA");
            std::string folderPath(appDataPath);
            return folderPath + R"(\corn-games\clangy-bird\settings\)";
        #endif

    #elif defined(__APPLE__) || defined(__MACH__)
        // macOS
        return "~/Library/Application Support/corn-games/clangy-bird/settings/";

    #elif defined(__linux__)
        // Linux
        const char* homeDir = getenv("HOME");
        if (homeDir == nullptr) return ""; // Handle error appropriately
        return std::string(homeDir) + "/.config/corn-games/clangy-bird/settings/";

    #else
        #error "Unknown platform"
    #endif
}

bool createSettings(const std::string& settingsRootFolder, const std::string& fileName, const std::string& fileContents) {
    try {
        // Create the directory and all its parent directories if they don't exist
        std::filesystem::create_directories(settingsRootFolder);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
    std::string settingsPath = settingsRootFolder + fileName;

    // Check if the settings.json file already exists
    std::ifstream fileCheck(settingsPath);
    // File exists, no need to create a new one
    if (fileCheck) return false;

    // File settings.json doesn't exist, create and write to it
    std::ofstream file(settingsPath);
    if (!file) return false;
    file << fileContents;
    return true;
}

TextManager::TextManager() : text(), settings() {
    // Read texts
    std::ifstream textFile("resources/text.json");
    std::string settingsRootFolder = getSettingsRootFolder();
    createSettings(settingsRootFolder, "settings.json",
                   "{\n"
                   "    \"lang\": \"eng\"\n"
                   "}\n");
    this->settingsPath = settingsRootFolder + "settings.json";
    std::ifstream settingsFile(this->settingsPath);
    textFile >> this->text;
    settingsFile >> this->settings;
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

void TextManager::changeSettings(std::string key, nlohmann::json val) noexcept {
    this->settings[std::move(key)] = std::move(val);
}

void TextManager::saveSettings() noexcept {
    std::ofstream settingsFile(this->settingsPath);
    settingsFile << this->settings.dump(4);
}

const nlohmann::json& TextManager::getSettings(const std::string& key) const {
    return this->settings.at(key);
}
