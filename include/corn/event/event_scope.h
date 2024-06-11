#pragma once

#include <vector>
#include <corn/event/event_manager.h>

namespace corn {
    /**
     * @class EventScope
     * @brief An RAII wrapper for registering and unregistering event listeners.
     *
     * @see EventManager
     */
    class EventScope {
    public:
        /// @brief Constructor.
        EventScope();

        /// @brief Destructor. Removes all event listeners.
        ~EventScope();

        EventScope(const EventScope& other) = delete;
        EventScope& operator=(const EventScope& other) = delete;

        /**
         * @brief Adds an event listener to the global event manager and return an unique ID of the event.
         * @param eventType Tag of the event.
         * @param listener Call back function which activates when an event with the same event type is emitted.
         * @return Event ID.
         */
        EventManager::ListenerID addListener(const std::string& eventType, const EventManager::Action& listener);

        /**
         * @brief Adds an event listener to the given event manager and return an unique ID of the event.
         * @param eventType Tag of the event.
         * @param listener Call back function which activates when an event with the same event type is emitted.
         * @return Event ID.
         */
        EventManager::ListenerID addListener(EventManager& eventManager, const std::string& eventType, const EventManager::Action& listener);

    private:
        std::vector<std::pair<std::string, EventManager::ListenerID>> listenerIDList_;
    };
}
