#include <corn/core.h>
#include <corn/media.h>
#include <corn/util.h>
#include "scenes.h"

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    config.width = WIDTH;
    config.height = HEIGHT;
    corn::FontManager::instance().load("noto-sans-zh", "fonts/noto-sans-zh/NotoSansSC-VariableFont_wght.ttf");
    corn::Game game(new GameScene(), config);
    game.run();
    corn::FontManager::instance().unload("consolas");
    return 0;
}
