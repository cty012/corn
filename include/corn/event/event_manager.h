#pragma once


#include <functional>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <corn/event/event_args.h>

namespace corn {
    /**
     * Singleton class that handles all events.
     */
    class EventManager {
    public:
        using ListenerID = int;
        using Action = std::function<void(const EventArgs&)>;

        /**
         * @return The singleton instance
         */
        static EventManager& instance();

        /**
         * Adds an event listener and return an unique id of the event
         * @param eventType Tag of the event
         * @param listener Call back function which activates when an event with the same event type is emitted
         * @return Event id
         */
        ListenerID addListener(const std::string& eventType, const Action& listener);

        /**
         * Removes an event listener by its ID
         * @param listenerId ID of the listener assigned when adding the event
         * @return Whether the listener is removed successfully
         */
        bool removeListener(ListenerID listenerId);

        /**
         * Emits an event with the given argument. Calls all listeners with the same event type.
         * @param args Contains all information about the event
         * @return The number of listeners being called
         */
        int emit(const EventArgs& args);

    private:
        std::map<std::string, std::vector<std::pair<ListenerID, Action>>> listeners;

        EventManager() = default;
        ~EventManager() = default;
        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        friend class Game;
    };
}
