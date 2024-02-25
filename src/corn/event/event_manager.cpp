#include <cstdio>
#include <corn/event/event_args.h>
#include <corn/event/event_manager.h>
#include <corn/util/exceptions.h>

namespace corn {
    EventManager::EventManager() : listeners_() {}

    EventManager::~EventManager() {
        for (EventManager* room : this->privateRooms_) {
            delete room;
        }
        for (auto& [roomName, room] : this->rooms_) {
            delete room;
        }
    }

    EventManager& EventManager::instance() noexcept {
        static EventManager instance;
        return instance;
    }

    EventManager& EventManager::instance(const std::string& room) {
        EventManager& instance = EventManager::instance();
        if (!instance.rooms_.contains(room))
            throw std::invalid_argument("Room \"" + room + "\" does not exist.");
        return *instance.rooms_[room];
    }

    EventManager& EventManager::privateInstance() {
        static EventManager instance;
        return instance;
    }

    bool EventManager::addRoom(const std::string& room) noexcept {
        EventManager& instance = EventManager::instance();
        if (instance.rooms_.contains(room)) return false;
        auto* manager = new EventManager();
        instance.rooms_[room] = manager;
        return true;
    }

    bool EventManager::removeRoom(const std::string& room) noexcept {
        EventManager& instance = EventManager::instance();
        if (!instance.rooms_.contains(room)) return false;
        delete instance.rooms_[room];
        instance.rooms_.erase(room);
        return true;
    }

    EventManager* EventManager::addPrivateRoom() noexcept {
        EventManager& instance = EventManager::privateInstance();
        auto* manager = new EventManager();
        instance.privateRooms_.push_back(manager);
        return manager;
    }

    bool EventManager::removePrivateRoom(EventManager* manager) noexcept {
        EventManager& instance = EventManager::privateInstance();
        std::vector<EventManager*>& rooms = instance.privateRooms_;
        auto newEnd = std::remove(rooms.begin(), rooms.end(), manager);
        if (newEnd == rooms.end()) return false;
        rooms.erase(newEnd, rooms.end());
        return true;
    }

    EventManager::ListenerID EventManager::addListener(const std::string &eventType, const Action& listener) noexcept {
        static EventManager::ListenerID listenerId = 0;
        EventManager::ListenerID currentId = listenerId++;
        this->listeners_[eventType].emplace_back(currentId, listener);
        return currentId;
    }

    bool EventManager::removeListener(EventManager::ListenerID listenerID) noexcept {
        bool result = false;
        for (auto& [eventType, callbacks] : this->listeners_) {
            size_t count = std::erase_if(callbacks, [listenerID](const auto& item) {
                return item.first == listenerID;
            });
            if (count > 0) result = true;
        }
        std::erase_if(this->listeners_, [](const auto& item) {
            return item.second.empty();
        });
        return result;
    }

    int EventManager::emit(const EventArgs& args, bool propagate) noexcept {  // NOLINT
        if (!this->listeners_.contains(args.type())) return 0;
        const std::vector<std::pair<ListenerID, Action>>& callbacks = this->listeners_.at(args.type());
        for (const auto& [listenerId, action] : callbacks) {
            try {
                action(args);
            } catch (const std::exception& e) {
                fprintf(stderr, "Exception occurred when emitting event [%s]: %s\n",
                        args.type(), e.what());
            }
        }

        // Propagate to sub-rooms
        if (propagate) {
            for (auto& [roomName, room]: this->rooms_) {
                room->emit(args, true);
            }
        }

        return (int)callbacks.size();
    }
}
