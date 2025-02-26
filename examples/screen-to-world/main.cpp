#include <corn/core.h>
#include <corn/event.h>
#include <corn/util.h>
#include "scenes.h"

int main() {
    // Config
    corn::Config config;
    config.title = "Screen to World";
    config.width = 1080;
    config.height = 720;
    config.antialiasing = 4;

    // Events
    corn::EventScope eventScope;
    eventScope.addListener(
            "corn::input::exit",
            [](const corn::EventArgs&) {
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });

    // Game
    corn::Game game(new MainScene(), config);
    game.run();

    // Release resources
    corn::FontManager::instance().unload("noto-sans");

    return 0;
}
