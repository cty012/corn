#include <corn/core.h>
#include <corn/media.h>
#include <corn/util.h>
#include "scenes.h"
#include "constants.h"

int main() {
    corn::Config config;
    config.title = "Clangy Bird";
    config.width = WIDTH;
    config.height = HEIGHT;
    config.antialiasing = 4;
    corn::FontManager::instance().load(
            "noto-sans-zh", "resources/fonts/noto-sans-zh/static/NotoSansSC-Regular.ttf");
    corn::Game game(new MainMenuScene(), config);
    game.run();
    corn::FontManager::instance().unload("noto-sans-zh");
    return 0;
}
