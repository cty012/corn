#include <corn/core/game.h>
#include <corn/media/font_manager.h>
#include "scenes/game.h"

int main() {
    // Config
    corn::Config config;
    config.title = "Cleaning Robot";
    config.width = 1080;
    config.height = 720;
    config.antialiasing = 4;
    corn::FontManager::instance().loadFromPath(
            "noto-sans", "resources/fonts/noto-sans/NotoSans-Regular.ttf");
    corn::FontManager::instance().setDefault("noto-sans");

    // Events
    corn::EventScope eventScope;
    eventScope.addListener(
            "corn::input::exit",
            [](const corn::EventArgs&) {
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });

    // Game
    corn::Game game(new GameScene(), config);
    game.setDebugOverlay(true);
    game.run();

    // Release resources
    corn::FontManager::instance().unload("noto-sans");

    return 0;
}
