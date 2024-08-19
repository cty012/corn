#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace corn {
    struct EventArgs;

    /**
     * @class EventManager
     * @brief Multiton class that handles all events.
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
     * @see EventScope
     */
    class EventManager {
    public:
        using ListenerID = unsigned long long int;
        using Action = std::function<void(const EventArgs&)>;

        /**
         * @return The multiton root instance (or global instance).
         *
         * Events emitted from this instance will propagate to all rooms.
         */
        [[nodiscard]] static EventManager& instance() noexcept;

        /**
         * @param room Identifier of the room.
         * @return Whether the specified room exist.
         */
        [[nodiscard]] static bool hasInstance(const std::string& room) noexcept;

        /**
         * @param room Identifier of the room.
         * @return An instance for the specified room.
         * @throw std::invalid_argument If the room doesn't exist.
         *
         * Will return the global instance if identifier is empty.
         */
        [[nodiscard]] static EventManager& instance(const std::string& room);

        /**
         * @brief Creates a new room.
         * @param room Identifier of the room.
         * @return Whether the room is created successfully.
         *
         * Identifier cannot be empty.
         */
        static bool addRoom(const std::string& room) noexcept;

        /**
         * @brief Removes a room.
         * @param room Identifier of the room.
         * @return Whether the room is removed successfully.
         */
        static bool removeRoom(const std::string& room) noexcept;

        /**
         * @brief Creates a new private room.
         * @return Whether the room is created successfully.
         *
         * A private room is isolated from the rest of the rooms, so events from the root will not propagate here.
         */
        static EventManager* addPrivateRoom() noexcept;

        /**
         * @brief Removes a room.
         * @param room Identifier of the room.
         * @return Whether the room is removed successfully.
         */
        static bool removePrivateRoom(EventManager* room) noexcept;

        /// @brief Get the name (identifier) of the event manager.
        [[nodiscard]] const std::string& getName() const noexcept;

        /// @brief Whether the event manager is public.
        [[nodiscard]] bool isPublic() const noexcept;

        /**
         * @brief Adds an event listener and return an unique ID of the event.
         * @param eventType Tag of the event.
         * @param listener Call back function which activates when an event with the same event type is emitted.
         * @return Event ID.
         */
        ListenerID addListener(const std::string& eventType, const Action& listener) noexcept;

        /**
         * @brief Removes an event listener by its ID.
         * @param listenerID ID of the listener assigned when adding the event.
         * @return Whether the listener is removed successfully.
         */
        bool removeListener(ListenerID listenerID) noexcept;

        /**
         * @brief Emits an event with the given argument. Calls all listeners with the same event type.
         * @param args Contains all information about the event.
         * @return The number of listeners being called.
         */
        int emit(const EventArgs& args) noexcept;

        /**
         * @brief Emits an event with the given argument. Calls all listeners with the same event type. Event is
         *        received by all public rooms.
         * @param args Contains all information about the event.
         * @return The number of listeners being called.
         */
        static int broadcast(const EventArgs& args) noexcept;

    private:
        /// @brief Constructor.
        EventManager(std::string name, bool isPublic);

        /// @brief Destructor.
        ~EventManager();

        EventManager(const EventManager& other) = delete;
        EventManager& operator=(const EventManager& other) = delete;

        /// @return The root of all private instances.
        [[nodiscard]] static EventManager& privateInstance();

        /// @brief Name (identifier) of the event manager.
        std::string name_;

        /// @brief Whether the event manager is public.
        bool isPublic_;

        /// @brief Map for storing all event listeners.
        std::unordered_map<std::string, std::vector<std::pair<ListenerID, Action>>> listeners_;

        /// @brief Map for storing all (public) sub-rooms.
        std::unordered_map<std::string, EventManager*> rooms_;

        /// @brief Vector for storing private sub-rooms.
        std::vector<EventManager*> privateRooms_;
    };
}
