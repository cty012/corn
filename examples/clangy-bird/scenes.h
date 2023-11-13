#pragma once

#include <corn/core.h>
#include <corn/ecs.h>
#include <corn/event.h>
#include "constants.h"

/// The main game scene
class GameScene : public corn::Scene {
public:
    GameScene();
    ~GameScene() override;

    bool isPaused() const;
    void togglePause();

private:
    bool paused;
    corn::Entity* bird;
    corn::CMovement2D* birdMovement;
    corn::EventManager::ListenerID keyboardEventID;
    corn::EventManager::ListenerID mouseEventID;

    void onKeyboardEvent(const corn::EventArgsKeyboard& args);
    void onMouseEvent(const corn::EventArgsMouseButton& args);
};
