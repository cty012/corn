#include <cstdio>
#include <corn/event/event_args.h>
#include <corn/event/event_manager.h>
#include <corn/util/exceptions.h>

namespace corn {
    EventManager::EventManager(std::string name, bool isPublic) : name_(std::move(name)), isPublic_(isPublic) {}

    EventManager::~EventManager() {
        for (EventManager* room : this->privateRooms_) {
            delete room;
        }
        for (auto& [roomName, room] : this->rooms_) {
            delete room;
        }
    }

    EventManager& EventManager::instance() noexcept {
        static EventManager instance("", true);
        return instance;
    }

    bool EventManager::hasInstance(const std::string& room) noexcept {
        if (room.empty()) return true;
        EventManager& instance = EventManager::instance();
        return instance.rooms_.contains(room);
    }

    EventManager& EventManager::instance(const std::string& room) {
        EventManager& instance = EventManager::instance();
        if (room.empty()) return instance;
        if (!instance.rooms_.contains(room))
            throw std::invalid_argument("Room \"" + room + "\" does not exist.");
        return *instance.rooms_[room];
    }

    EventManager& EventManager::privateInstance() {
        static EventManager instance("", false);
        return instance;
    }

    bool EventManager::addRoom(const std::string& room) noexcept {
        EventManager& instance = EventManager::instance();
        if (room.empty() || instance.rooms_.contains(room)) return false;
        instance.rooms_[room] = new EventManager(room, true);
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
        auto* manager = new EventManager("", false);
        instance.privateRooms_.push_back(manager);
        return manager;
    }

    bool EventManager::removePrivateRoom(EventManager* manager) noexcept {
        EventManager& instance = EventManager::privateInstance();
        std::vector<EventManager*>& rooms = instance.privateRooms_;
        size_t original_size = rooms.size();
        delete manager;
        std::erase(rooms, manager);
        return original_size != rooms.size();
    }

    const std::string& EventManager::getName() const noexcept {
        return this->name_;
    }

    bool EventManager::isPublic() const noexcept {
        return this->isPublic_;
    }

    EventManager::ListenerID EventManager::addListener(const std::string& eventType, const Action& listener) noexcept {
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

    int EventManager::emit(const EventArgs& args) noexcept {
        if (!this->listeners_.contains(args.type())) return 0;
        // Callbacks are copied to avoid changes to them when invoking the actions.
        std::vector<std::pair<ListenerID, Action>> callbacks = this->listeners_.at(args.type());
        for (const auto& [listenerId, action] : callbacks) {
            try {
                action(args);
            } catch (const std::exception& e) {
                fprintf(stderr, "Exception occurred when emitting event [%s]: %s\n",
                        args.type().c_str(), e.what());
            }
        }

        return (int)callbacks.size();
    }

    int EventManager::broadcast(const EventArgs& args) noexcept {
        int count = 0;

        // Emit to root room
        EventManager::instance().emit(args);

        // Propagate to all public rooms
        for (auto& [roomName, room]: EventManager::instance().rooms_) {
            count += room->emit(args);
        }

        return count;
    }
}
