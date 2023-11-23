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
    corn::EventManager::ListenerID langChangeEventID;
};

/// The settings scene
class SettingsScene : public corn::Scene {
public:
    SettingsScene();
    ~SettingsScene() override;

private:
    corn::EventManager::ListenerID langChangeEventID;
};

/// The main game scene
class GameScene : public corn::Scene {
public:
    GameScene();
    ~GameScene() override;

    [[nodiscard]] bool isPaused() const;
    void togglePause();

private:
    bool paused;
    corn::UIWidget* pauseMenu;
    corn::Entity* bird;
    corn::CMovement2D* birdMovement;
    corn::EventManager::ListenerID keyboardEventID;
    corn::EventManager::ListenerID mouseEventID;
    corn::EventManager::ListenerID langChangeEventID;

    void onKeyboardEvent(const corn::EventArgsKeyboard& args);
    void onMouseEvent(const corn::EventArgsMouseButton& args);
};
