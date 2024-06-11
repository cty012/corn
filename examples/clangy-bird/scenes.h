#pragma once

#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include "constants.h"

/// The menu scene
class MainMenuScene : public corn::Scene {
public:
    MainMenuScene();
    ~MainMenuScene() override;

private:
    corn::EventScope eventScope_;
};

/// The settings scene
class SettingsScene : public corn::Scene {
public:
    SettingsScene();
    ~SettingsScene() override;

private:
    corn::EventScope eventScope_;
};

/// The tutorial scene
class TutorialScene : public corn::Scene {
public:
    TutorialScene();
    ~TutorialScene() override;

private:
    corn::EventScope eventScope_;
};

/// The main game scene
class GameScene : public corn::Scene {
public:
    GameScene();
    ~GameScene() override;

    [[nodiscard]] bool isPaused() const;
    void togglePause();

private:
    void onKeyboardEvent(const corn::EventArgsKeyboard& args);
    void onMouseEvent(const corn::EventArgsMouseButton& args);

    bool paused_;
    corn::UIWidget* pauseMenu_;
    corn::Entity* bird_;
    corn::CMovement2D* birdMovement_;
    std::vector<corn::System*> addedSystems_;
    corn::EventScope eventScope_;
};
