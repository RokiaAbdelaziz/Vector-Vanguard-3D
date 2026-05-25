#pragma once
#include "Event.h"
#include "raylib.h"
class WeaponFiredEvent : public Event {
public:
    Vector3 origin;
    float power;

    WeaponFiredEvent(Vector3 pos, float p) : origin(pos), power(p) {}
    EventType GetType() const override { return EventType::WeaponFired; }
};