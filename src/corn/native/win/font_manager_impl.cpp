#include <ranges>
#include "win/font_collection_loader.h"
#include "win/font_manager_impl.h"

namespace corn {
    FontManager::FontManagerImpl::FontManagerImpl() {
        this->fontCollectionLoader = new SingleFontCollectionLoader();
        getDWriteFactory5()->RegisterFontCollectionLoader(this->fontCollectionLoader);
    }

    FontManager::FontManagerImpl::~FontManagerImpl() {
        getDWriteFactory5()->UnregisterFontCollectionLoader(this->fontCollectionLoader);
        this->fontCollectionLoader->Release();
    }
}
