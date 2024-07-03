#include <gtest/gtest.h>
#include <corn/event/event_scope.h>
#include "event_args_dummy.h"

namespace corn::test {
    TEST(EventScope, raii) {
        bool listenerStillExist;

        // Root - emit within scope
        listenerStillExist = false;
        {
            EventScope eventScope;
            eventScope.addListener(
                    "corn::test::dummy",
                    [&listenerStillExist](const EventArgs&) {
                        listenerStillExist = true;
                    });
            EventManager::instance().emit(EventArgsDummy());
        }
        EXPECT_EQ(listenerStillExist, true);

        // Root - emit outside scope
        listenerStillExist = false;
        {
            EventScope eventScope;
            eventScope.addListener(
                    "corn::test::dummy",
                    [&listenerStillExist](const EventArgs&) {
                        listenerStillExist = true;
                    });
        }
        EventManager::instance().emit(EventArgsDummy());
        EXPECT_EQ(listenerStillExist, false);

        // Public room - emit within scope
        listenerStillExist = false;
        EventManager::addRoom("event-scope1");
        EventManager& room1 = EventManager::instance("event-scope1");
        {
            EventScope eventScope;
            eventScope.addListener(
                    room1,
                    "corn::test::dummy",
                    [&listenerStillExist](const EventArgs&) {
                        listenerStillExist = true;
                    });
            room1.emit(EventArgsDummy());
        }
        EventManager::removeRoom("event-scope1");
        EXPECT_EQ(listenerStillExist, true);

        // Public room - emit outside scope
        listenerStillExist = false;
        EventManager::addRoom("event-scope2");
        EventManager& room2 = EventManager::instance("event-scope2");
        {
            EventScope eventScope;
            eventScope.addListener(
                    room2,
                    "corn::test::dummy",
                    [&listenerStillExist](const EventArgs&) {
                        listenerStillExist = true;
                    });
        }
        room2.emit(EventArgsDummy());
        EventManager::removeRoom("event-scope2");
        EXPECT_EQ(listenerStillExist, false);
    }
}
