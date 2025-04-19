#include <corn/media/font_manager.h>

namespace corn {
    FontManager& FontManager::instance() {
        static FontManager instance;
        return instance;
    }
}
