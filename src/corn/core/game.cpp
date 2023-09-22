#include <corn/core/game.h>

#include <utility>

namespace corn {
    Game::Game(Scene* startScene, Config config)
        : active(false), scenes(std::stack<Scene*>()), config(std::move(config)) {

        this->scenes.push(startScene);

        // Register event listeners
        this->sceneEventId = EventManager::instance().addListener(
                "corn::game::scene", [this](const EventArgs &args) {
                    this->onSceneEvent(dynamic_cast<const EventArgsScene&>(args));
                });
        this->closeEventId = EventManager::instance().addListener(
                "corn::input::exit", [this](const EventArgs &args) {
                    this->onExitEvent(dynamic_cast<const EventArgsExit&>(args));
                });

        this->interface = new Interface(&this->config);
        this->interface->init();
    }

    const Config& Game::getConfig() const {
        return this->config;
    }

    void Game::setConfig(Config newConfig) {
        this->config = std::move(newConfig);
        // TODO: reload settings
    }

    Game::~Game() {
        // Unregister event listeners
        EventManager::instance().removeListener(this->sceneEventId);
        EventManager::instance().removeListener(this->closeEventId);
        // Deallocation
        delete this->interface;
        this->removeAllScenes();
        while (!this->sceneEvents.empty()) {
            delete this->sceneEvents.front().getScene();
            this->sceneEvents.pop();
        }
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
        this->sceneEvents.push(args);
    }

    void Game::resolveSceneEvents() {
        while (!this->sceneEvents.empty()) {
            EventArgsScene& args = this->sceneEvents.front();
            this->changeScene(args.getOperation(), args.getScene());
            this->sceneEvents.pop();
        }
    }

    void Game::onExitEvent(const EventArgsExit& args) {
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

            // Update scene stack
            this->resolveSceneEvents();
        }
        return 0;
    }
}
