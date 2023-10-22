#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <corn/event/event_args.h>

namespace corn {
    /**
     * @class EventManager
     * @brief Singleton class that handles all events.
     *
     * Events are defined by extending the EventArgs class. To listen to an event, use the addListener method. The
     * listener ID returned can be used for unregistering the event listener. To emit an event, use the emit method
     * with arguments stored in the corresponding child class of EventArgs defined earlier.
     *
     * Events and event listeners are matched by a std::string called event type. When registering a listener, the
     * event type is being passed as an argument. When emitting an event, the event type is the return value of
     * type() method of the event argument.
     *
     * @see EventArgs
     */
    class EventManager {
    public:
        using ListenerID = unsigned long long int;
        using Action = std::function<void(const EventArgs&)>;

        /// @return The singleton instance.
        static EventManager& instance();

        /**
         * @brief Adds an event listener and return an unique id of the event.
         * @param eventType Tag of the event.
         * @param listener Call back function which activates when an event with the same event type is emitted.
         * @return Event id
         */
        ListenerID addListener(const std::string& eventType, const Action& listener);

        /**
         * @brief Removes an event listener by its ID.
         * @param listenerId ID of the listener assigned when adding the event.
         * @return Whether the listener is removed successfully.
         */
        bool removeListener(ListenerID listenerId);

        /**
         * @brief Emits an event with the given argument. Calls all listeners with the same event type.
         * @param args Contains all information about the event.
         * @return The number of listeners being called.
         */
        int emit(const EventArgs& args);

    private:
        std::unordered_map<std::string, std::vector<std::pair<ListenerID, Action>>> listeners;

        // Constructors and destructors are hidden to maintain a singleton pattern.
        EventManager();
        ~EventManager() = default;
        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;
    };
}
