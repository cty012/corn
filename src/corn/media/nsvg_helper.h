#pragma once

#include <filesystem>
#include <string>
#include <nanosvg.h>

bool readFromSVGFile(const std::filesystem::path& path, std::string& output);

NSVGimage* createSVGImage(const std::string& svgContent, const std::string& units, float dpi);
