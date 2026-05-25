#include "EventManager.h"

void EventManager::AddListener(EventType type, EventCallback listener) {
    listeners[type].push_back(listener);
}
void EventManager::TriggerEvent(const Event& event) {
    EventType type = event.GetType();
    if (listeners.count(type)) {
        for (auto& callback : listeners[type]) {
            callback(event);
        }
    }
}