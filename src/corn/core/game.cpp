#include <thread>
#include <corn/core/game.h>

namespace corn {
    Game::Game(Scene* startScene, Config config)
        : active(false), config(std::move(config)), scenes(std::stack<Scene*>()), sw(Stopwatch()) {

        this->scenes.push(startScene);

        // Register event listeners
        this->sceneEventID = EventManager::instance().addListener(
                "corn::game::scene", [this](const EventArgs& args) {
                    this->onSceneEvent(dynamic_cast<const EventArgsScene&>(args));
                });
        this->closeEventID = EventManager::instance().addListener(
                "corn::input::exit", [this](const EventArgs& args) {
                    this->onExitEvent(dynamic_cast<const EventArgsExit&>(args));
                });

        this->interface = new Interface(&this->config);
        this->interface->init();
    }

    Game::~Game() {
        // Unregister event listeners
        EventManager::instance().removeListener(this->sceneEventID);
        EventManager::instance().removeListener(this->closeEventID);

        // Deallocation
        delete this->interface;
        this->removeAllScenes();
        while (!this->sceneEvents.empty()) {
            delete this->sceneEvents.front().scene;
            this->sceneEvents.pop();
        }
    }

    const Config& Game::getConfig() const {
        return this->config;
    }

    void Game::setConfig(Config newConfig) {
        this->config = std::move(newConfig);
        // TODO: reload settings
    }

    void Game::changeScene(corn::SceneOperation op, corn::Scene* scene) {
        switch (op) {
            case SceneOperation::PUSH:  // Add new scene to top
                this->scenes.push(scene);
                break;
            case SceneOperation::POP:  // Remove the top scene
                this->removeOneScene();
                delete scene;
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
            this->changeScene(args.op, args.scene);
            this->sceneEvents.pop();
        }
    }

    void Game::onExitEvent(const EventArgsExit& args) {
        (void)args;
        this->active = false;
    }

    int Game::run() {
        this->active = true;
        this->sw.clear();  // Just in case
        this->sw.play();
        while (this->active && !this->scenes.empty()) {
            // Get millis
            float millis = this->sw.millis();
            this->sw.clear();
            this->sw.play();

            // Update scene
            this->scenes.top()->update(millis);

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
