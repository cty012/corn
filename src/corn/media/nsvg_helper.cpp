#include <fstream>
#include "nsvg_helper.h"

bool readFromSVGFile(const std::filesystem::path& filename, std::string& output) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    // Read file contents into a string.
    output = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return true;
}

NSVGimage* createSVGImage(const std::string& svgContent, const std::string& units, float dpi) {
    auto* svgContentCStr = new char[svgContent.size() + 1];
    memcpy(svgContentCStr, svgContent.c_str(), svgContent.size());
    svgContentCStr[svgContent.size()] = '\0';
    NSVGimage* image = nsvgParse(svgContentCStr, units.c_str(), dpi);
    delete[] svgContentCStr;
    return image;
}
