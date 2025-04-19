#pragma once

#include <corn/media/font_manager.h>
#include "win/utils.h"

namespace corn {
    class FontManager::FontManagerImpl {
    public:
        IDWriteFontCollectionLoader* fontCollectionLoader;

        FontManagerImpl();
        ~FontManagerImpl();
        FontManagerImpl(const FontManagerImpl&) = delete;
        FontManagerImpl& operator=(const FontManagerImpl&) = delete;
    };
}
