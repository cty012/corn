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
    corn::FontManager::instance().load(
            "noto-sans-zh", "resources/fonts/noto-sans-zh/static/NotoSansSC-Regular.ttf");

    // Events
    corn::EventScope eventScope;
    eventScope.addListener(
            "corn::input::exit",
            [](const corn::EventArgs&) {
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });

    // Game
    corn::Game game(new MainMenuScene(), config);
    game.run();

    // Release resources
    corn::FontManager::instance().unload("noto-sans-zh");

    return 0;
}
