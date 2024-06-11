#include <corn/core.h>
#include <corn/event.h>
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

    corn::EventManager::ListenerID exitEventID = corn::EventManager::instance().addListener(
            "corn::input::exit",
            [](const corn::EventArgs&) {
                corn::EventManager::instance().emit(corn::EventArgsExit());
            });

    corn::Game game(new MainMenuScene(), config);
    game.run();

    corn::EventManager::instance().removeListener(exitEventID);
    corn::FontManager::instance().unload("noto-sans-zh");
    return 0;
}
