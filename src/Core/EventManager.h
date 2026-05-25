#pragma once
#include <vector>
#include <map>
#include <functional>
#include "Event.h"
using EventCallback = std::function<void(const Event&)>;

class EventManager {
private:
    EventManager() {}
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
    std::map<EventType, std::vector<EventCallback>> listeners;

public:
    static EventManager& Instance() {
        static EventManager instance;
        return instance;
    }

    void AddListener(EventType type, EventCallback listener);
    void TriggerEvent(const Event& event);
};