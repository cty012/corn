#include <corn/core/game.h>
#include <corn/core/scene.h>
#include <corn/media/interface.h>

namespace corn {
    Game::Game(Scene* startScene, Config config)
            : active_(false), config_(std::move(config)), scenes_(), keyPressed_(), sw_(), debugOverlayEnabled_(false) {

        startScene->game_ = this;
        this->scenes_.push(startScene);

        // Register event listeners
        this->eventScope_.addListener(
                "corn::game::scene",
                [this](const EventArgs& args) {
                    this->sceneEvents_.push(dynamic_cast<const EventArgsScene&>(args));
                });

        this->eventScope_.addListener(
                "corn::exit",
                [this](const EventArgs&) {
                    this->active_ = false;
                });

        this->interface_ = new Interface(*this, this->keyPressed_);
        this->interface_->init();
    }

    Game::~Game() {
        // Deallocation
        delete this->interface_;
        this->removeAllScenes();
        while (!this->sceneEvents_.empty()) {
            delete this->sceneEvents_.front().scene;
            this->sceneEvents_.pop();
        }
    }

    const Config& Game::getConfig() const noexcept {
        return this->config_;
    }

    void Game::setConfig(Config config) {
        this->config_ = std::move(config);
        this->interface_->init();
    }

    void Game::setDebugOverlay(bool debugOverlayEnabled) noexcept {
        this->debugOverlayEnabled_ = debugOverlayEnabled;
    }

    Vec2 Game::windowSize() const noexcept {
        return this->interface_->windowSize();
    }

    Scene* Game::getTopScene() const noexcept {
        return this->scenes_.empty() ? nullptr : this->scenes_.top();
    }

    bool Game::isPressed(Key key) const noexcept {
        return this->keyPressed_.contains(key) && this->keyPressed_.at(key);
    }

    void Game::changeScene(corn::SceneOperation op, corn::Scene* scene) noexcept {
        switch (op) {
            case SceneOperation::PUSH:  // Add new scene to top
                if (scene) {
                    scene->game_ = this;
                    this->scenes_.push(scene);
                }
                break;
            case SceneOperation::POP:  // Remove the top scene
                this->removeOneScene();
                break;
            case SceneOperation::REPLACE:  // Remove the top scene and add new scene to top
                if (scene) {
                    this->removeOneScene();
                    scene->game_ = this;
                    this->scenes_.push(scene);
                }
                break;
            case SceneOperation::REPLACE_ALL:  // Remove all scenes and add new scene to top
                if (scene) {
                    this->removeAllScenes();
                    scene->game_ = this;
                    this->scenes_.push(scene);
                }
                break;
        }
    }

    bool Game::removeOneScene() noexcept {
        if (this->scenes_.empty()) return false;
        delete this->scenes_.top();
        this->scenes_.pop();
        return true;
    }

    size_t Game::removeAllScenes() noexcept {
        size_t result = this->scenes_.size();
        while (!this->scenes_.empty()) {
            delete this->scenes_.top();
            this->scenes_.pop();
        }
        return result;
    }

    void Game::resolveSceneEvents() noexcept {
        while (!this->sceneEvents_.empty()) {
            EventArgsScene& args = this->sceneEvents_.front();
            this->changeScene(args.op, args.scene);
            this->sceneEvents_.pop();
        }
    }

    void Game::run() {
        this->active_ = true;
        this->sw_.clear();  // Just in case
        this->sw_.play();

        bool debugDataAvailable = false;
        float debugDataRefreshRate = 500.0f;
        float debugDataRefreshProgress = 0.0f;

        // FPS data
        size_t nFrames = 0;
        size_t fps = 0;

        while (this->active_ && !this->scenes_.empty()) {
            // Get millis
            float millis = this->sw_.millis();
            this->sw_.clear();
            this->sw_.play();

            // Update debug data
            debugDataRefreshProgress += millis;
            nFrames++;
            if (debugDataRefreshProgress >= debugDataRefreshRate) {
                debugDataAvailable = true;
                fps = (size_t)std::round((float)nFrames / debugDataRefreshProgress * 1000.0f);
                debugDataRefreshProgress = 0.0f;
                nFrames = 0;
            }

            // Update scene
            this->scenes_.top()->update(millis);

            // Update interface
            this->interface_->handleUserInput();
            this->interface_->render(this->scenes_.top());
            if (this->debugOverlayEnabled_ && debugDataAvailable) {
                this->interface_->renderDebugOverlay(fps);
            }
            this->interface_->update();

            // Update scene stack
            this->resolveSceneEvents();
        }
    }
}
