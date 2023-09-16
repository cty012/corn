#include <corn/core/game.h>

namespace corn {
    Game::Game(Scene* startScene, Config* config): active(false), scenes(std::stack<Scene*>()) {
        this->scenes.push(startScene);

        // Register event listeners
        this->sceneEventId = EventManager::instance().addListener(
                "corn::game::scene", [this](const EventArgs &args) {
                    this->onSceneEvent(dynamic_cast<const EventArgsScene&>(args));
                });
        this->closeEventId = EventManager::instance().addListener(
                "corn::input::close", [this](const EventArgs &args) {
                    this->onCloseEvent(dynamic_cast<const EventArgsClose&>(args));
                });

        // Use default settings if not specified
        this->config = config != nullptr ? config : new Config();
        this->interface = new Interface(this->config);
        this->interface->init();
    }

    Game::~Game() {
        delete this->interface;
        delete this->config;
        this->removeAllScenes();
        // Unregister event listeners
        EventManager::instance().removeListener(this->sceneEventId);
        EventManager::instance().removeListener(this->closeEventId);
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
        }
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

    void Game::onSceneEvent(const EventArgsScene& args) {
        this->changeScene(args.getOperation(), args.getScene());
    }

    void Game::onCloseEvent(const EventArgsClose& args) {
        this->active = false;
    }

    int Game::run() {
        this->active = true;
        while (this->active && !this->scenes.empty()) {
            // Update scene
            this->scenes.top()->update();
            // Update interface
            this->interface->handleUserInput();
            this->interface->render(this->scenes.top());
            this->interface->update();
        }
        return 0;
    }
}
