#include <cstdio>
#include <corn/event/event_manager.h>
#include <corn/util/exceptions.h>


namespace corn {
    EventManager::EventManager() : listeners() {}

    EventManager& EventManager::instance() {
        static EventManager instance;
        return instance;
    }

    EventManager::ListenerID EventManager::addListener(const std::string &eventType, const Action& listener) {
        static EventManager::ListenerID listenerId = 0;
        EventManager::ListenerID currentId = listenerId++;
        if (!this->listeners.contains(eventType)) {
            this->listeners[eventType] = std::vector<std::pair<ListenerID, Action>>();
        }
        this->listeners[eventType].emplace_back(currentId, listener);
        return currentId;
    }

    bool EventManager::removeListener(EventManager::ListenerID listenerId) {
        bool result = false;
        for (auto& [eventType, callbacks] : this->listeners) {
            size_t count = std::erase_if(callbacks, [listenerId](const auto& item) {
                return item.first == listenerId;
            });
            if (count > 0) result = true;
        }
        std::erase_if(this->listeners, [](const auto& item) {
            return item.second.empty();
        });
        return result;
    }

    int EventManager::emit(const EventArgs& args) {
        if (!this->listeners.contains(args.type())) return 0;
        const std::vector<std::pair<ListenerID, Action>>& callbacks = this->listeners.at(args.type());
        for (const auto& [listenerId, action] : callbacks) {
            try {
                action(args);
            } catch (const std::exception& e) {
                fprintf(stderr, "Exception occurred when emitting event [%s]: %s",
                        args.type().c_str(), e.what());
            }
        }
        return (int)callbacks.size();
    }
}
