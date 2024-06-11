#include <corn/event/event_scope.h>

namespace corn {
    EventScope::EventScope() = default;

    EventScope::~EventScope() {
        for (const auto& [roomName, listenerID] : this->listenerIDList_) {
            if (EventManager::hasInstance(roomName)) {
                EventManager::instance(roomName).removeListener(listenerID);
            }
        }
    }

    EventManager::ListenerID EventScope::addListener(
            const std::string& eventType, const EventManager::Action& listener) {

        EventManager::ListenerID listenerID = EventManager::instance().addListener(eventType, listener);
        this->listenerIDList_.emplace_back("", listenerID);
        return listenerID;
    }

    EventManager::ListenerID EventScope::addListener(
            EventManager& eventManager, const std::string& eventType, const EventManager::Action& listener) {

        EventManager::ListenerID listenerID = eventManager.addListener(eventType, listener);
        this->listenerIDList_.emplace_back(eventManager.getName(), listenerID);
        return listenerID;
    }
}
