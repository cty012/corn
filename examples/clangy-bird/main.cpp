#include <corn/core.h>
#include <corn/event.h>
#include <corn/util.h>
#include "scenes.h"
#include "constants.h"

int main() {
    // Config
    corn::Config config;
    config.title = "Clangy Bird";
    config.width = WIDTH;
    config.height = HEIGHT;
    config.antialiasing = 4;

    // Fonts
    corn::FontManager::instance().loadFontFamilyFromPath(
            "noto-sans-zh", "resources/fonts/noto-sans-zh/static/NotoSansSC-Regular.ttf");
    // TODO: The following code is for testing only, remove them later
    // corn::FontManager::instance().loadFontFamilyFromSystem("segoe-ui", "Segoe UI");
    // corn::FontManager::instance().loadFontFamilyFromSystem("yahei", "Microsoft YaHei");
    // corn::FontManager::instance().loadFontFamilyFromSystem("segoe-ui-emoji", "Segoe UI Emoji");

    // Events
    corn::EventScope eventScope;
    eventScope.addListener(
            "corn::input::exit",
            [](const corn::EventArgs&) {
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });

    // Game
    corn::Game game(new MainMenuScene(), config);
    game.setDebugOverlay(true);
    game.run();

    return 0;
}
