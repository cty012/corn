#include <thread>
#include <corn/core/game.h>

namespace corn {
    Game::Game(Scene* startScene, Config config)
            : active_(false), config_(std::move(config)), scenes_(), keyPressed_(), sw_() {

        this->scenes_.push(startScene);

        // Register event listeners
        this->sceneEventID_ = EventManager::instance().addListener(
                "corn::game::scene", [this](const EventArgs& args) {
                    this->sceneEvents_.push(dynamic_cast<const EventArgsScene&>(args));
                });
        this->closeEventID_ = EventManager::instance().addListener(
                "corn::input::exit", [this](const EventArgs&) {
                    this->active_ = false;
                });

        this->interface_ = new Interface(*this, this->keyPressed_);
        this->interface_->init();
    }

    Game::~Game() {
        // Unregister event listeners
        EventManager::instance().removeListener(this->sceneEventID_);
        EventManager::instance().removeListener(this->closeEventID_);

        // Deallocation
        delete this->interface_;
        this->removeAllScenes();
        while (!this->sceneEvents_.empty()) {
            delete this->sceneEvents_.front().scene;
            this->sceneEvents_.pop();
        }
    }

    const Config& Game::getConfig() const {
        return this->config_;
    }

    void Game::setConfig(Config newConfig) {
        this->config_ = std::move(newConfig);
        // TODO: reload settings
    }

    Scene* Game::getTopScene() const {
        return this->scenes_.empty() ? nullptr : this->scenes_.top();
    }

    const std::unordered_map<Key, bool>& Game::getKeyPressed() const {
        return this->keyPressed_;
    }

    void Game::changeScene(corn::SceneOperation op, corn::Scene* scene) {
        switch (op) {
            case SceneOperation::PUSH:  // Add new scene to top
                scene->game_ = this;
                this->scenes_.push(scene);
                break;
            case SceneOperation::POP:  // Remove the top scene
                this->removeOneScene();
                delete scene;
                break;
            case SceneOperation::REPLACE:  // Remove the top scene and add new scene to top
                this->removeOneScene();
                scene->game_ = this;
                this->scenes_.push(scene);
                break;
            case SceneOperation::REPLACE_ALL:  // Remove all scenes and add new scene to top
                this->removeAllScenes();
                scene->game_ = this;
                this->scenes_.push(scene);
                break;
        }
    }

    bool Game::removeOneScene() {
        if (this->scenes_.empty()) return false;
        delete this->scenes_.top();
        this->scenes_.pop();
        return true;
    }

    size_t Game::removeAllScenes() {
        size_t result = this->scenes_.size();
        while (!this->scenes_.empty()) {
            delete this->scenes_.top();
            this->scenes_.pop();
        }
        return result;
    }

    void Game::resolveSceneEvents() {
        while (!this->sceneEvents_.empty()) {
            EventArgsScene& args = this->sceneEvents_.front();
            this->changeScene(args.op, args.scene);
            this->sceneEvents_.pop();
        }
    }

    int Game::run() {
        this->active_ = true;
        this->sw_.clear();  // Just in case
        this->sw_.play();
        while (this->active_ && !this->scenes_.empty()) {
            // Get millis
            float millis = this->sw_.millis();
            this->sw_.clear();
            this->sw_.play();

            // Update scene
            this->scenes_.top()->update(millis);

            // Update interface
            this->interface_->handleUserInput();
            this->interface_->render(this->scenes_.top());
            this->interface_->update();

            // Update scene stack
            this->resolveSceneEvents();
        }
        return 0;
    }
}
