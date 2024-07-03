#include <gtest/gtest.h>
#include <corn/event/event_manager.h>
#include "dummy_event_arg.h"

namespace corn::test::event_manager {
    TEST(EventManager, room) {
        // Singleton
        EventManager& singleton = EventManager::instance();
        EXPECT_EQ(&EventManager::instance(), &singleton);

        // Room name
        EXPECT_EQ(EventManager::addRoom("room1"), true);
        EXPECT_EQ(EventManager::addRoom("room2"), true);
        EXPECT_EQ(EventManager::addRoom(""), false);

        // Public room
        EventManager& room1 = EventManager::instance("room1");
        EventManager& room2 = EventManager::instance("room2");
        EXPECT_NE(&room1, &singleton);
        EXPECT_NE(&room1, &room2);
        EXPECT_EQ(&EventManager::instance("room1"), &room1);
        EXPECT_ANY_THROW((void)EventManager::instance("does-not-exist"));
        EventManager::removeRoom("room1");
        EXPECT_NO_THROW(EventManager::removeRoom("room1"));
        EventManager::removeRoom("room2");

        // Private room
        EventManager* _room1 = nullptr;
        EXPECT_NO_THROW(_room1 = EventManager::addPrivateRoom());
        EXPECT_NO_THROW(EventManager::removePrivateRoom(_room1));
        EXPECT_NO_THROW(EventManager::removePrivateRoom(nullptr));
    }

    TEST(EventManager, emit) {
        // Setup
        EventManager::addRoom("emit1");
        EventManager& publicRoom = EventManager::instance("emit1");
        EventManager& privateRoom = *EventManager::addPrivateRoom();

        bool rootReceivesEvent, publicReceivesEvent, privateReceivesEvent;
        std::function<void(void)> reset = [&rootReceivesEvent, &publicReceivesEvent, &privateReceivesEvent]() {
            rootReceivesEvent = publicReceivesEvent = privateReceivesEvent = false;
        };

        // Add listeners
        EventManager::ListenerID rootListener = EventManager::instance().addListener(
                "corn::test::dummy",
                [&rootReceivesEvent](const EventArgs&) {
                    rootReceivesEvent = true;
                });
        publicRoom.addListener(
                "corn::test::dummy",
                [&publicReceivesEvent](const EventArgs&) {
                    publicReceivesEvent = true;
                });
        privateRoom.addListener(
                "corn::test::dummy",
                [&privateReceivesEvent](const EventArgs&) {
                    privateReceivesEvent = true;
                });

        // Emit global event
        reset();
        EventManager::instance().emit(EventArgsDummy());
        EXPECT_EQ(rootReceivesEvent, true);
        EXPECT_EQ(publicReceivesEvent, false);
        EXPECT_EQ(privateReceivesEvent, false);

        // Emit public room event
        reset();
        publicRoom.emit(EventArgsDummy());
        EXPECT_EQ(rootReceivesEvent, false);
        EXPECT_EQ(publicReceivesEvent, true);
        EXPECT_EQ(privateReceivesEvent, false);

        // Emit private room event
        reset();
        privateRoom.emit(EventArgsDummy());
        EXPECT_EQ(rootReceivesEvent, false);
        EXPECT_EQ(publicReceivesEvent, false);
        EXPECT_EQ(privateReceivesEvent, true);

        // Emit broadcast event
        reset();
        EventManager::broadcast(EventArgsDummy());
        EXPECT_EQ(rootReceivesEvent, true);
        EXPECT_EQ(publicReceivesEvent, true);
        EXPECT_EQ(privateReceivesEvent, false);

        // Cleanup
        EventManager::removeRoom("emit1");
        EventManager::removePrivateRoom(&privateRoom);
        EventManager::instance().removeListener(rootListener);
    }
}
