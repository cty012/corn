#include <stdexcept>
#include <corn/core/game.h>

namespace corn {
    Game::Game(Scene* startScene): scenes(std::stack<Scene*>()) {
        this->scenes.push(startScene);
        // Register event listeners
        this->sceneEventId = EventManager::instance().addListener(
                "corn::game::scene", [this](const EventArgs &args) {
                    this->onSceneEvent(dynamic_cast<const EventArgsScene&>(args));
                });
    }

    Game::~Game() {
        this->removeAllScenes();
        // Unregister event listeners
        EventManager::instance().removeListener(this->sceneEventId);
    }

    void Game::changeScene(corn::SceneOperation op, corn::Scene *scene) {
        switch (op) {
            case SceneOperation::PUSH:  // Add new scene to top
                this->scenes.push(scene);
                break;
            case SceneOperation::POP:  // Remove the top scene
                this->removeOneScene();
                break;
            case SceneOperation::REPLACE:  // Remove the top scene and add new scene to top
                this->removeOneScene();
                this->scenes.push(scene);
                break;
            case SceneOperation::REPLACE_ALL:  // Remove all scenes and add new scene to top
                this->removeAllScenes();
                this->scenes.push(scene);
                break;
            default:
                throw std::invalid_argument("Invalid argument for scene event");
        }
    }

    void Game::onSceneEvent(const EventArgsScene& args) {
        this->changeScene(args.getOperation(), args.getScene());
    }

    bool Game::removeOneScene() {
        if (this->scenes.empty()) return false;
        delete this->scenes.top();
        this->scenes.pop();
        return true;
    }

    size_t Game::removeAllScenes() {
        size_t result = this->scenes.size();
        while (!this->scenes.empty()) {
            delete this->scenes.top();
            this->scenes.pop();
        }
        return result;
    }

    int Game::run() {
        while (true) {
            if (this->scenes.empty()) break;
            // Update scene
            this->scenes.top()->update();
            // Update game-level systems
            // TODO: user input
            // TODO: render
        }
        return 0;
    }
}
