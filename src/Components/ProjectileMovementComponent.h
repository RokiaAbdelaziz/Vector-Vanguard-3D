#pragma once
#include "Component.h"
#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>

class GameObject;

class ProjectileMovementComponent : public Component {
public:
    float speed;
    float lifetime;
    float timer;
    Vector3 direction;
    ProjectileMovementComponent(GameObject* owner)
        : Component(owner), speed(0), lifetime(0), timer(0), direction({ 0,0,0 }) {
    }
    static Component* CreateComponent(void* pData, GameObject* pOwner);
    void Update(float deltaTime) override;
    void Render() override {}
    void SetSpeed(float s) { speed = s; }
    void SetLifetime(float l) { lifetime = l; }
    void SetDirection(Vector3 dir) { direction = Vector3Normalize(dir); }
};