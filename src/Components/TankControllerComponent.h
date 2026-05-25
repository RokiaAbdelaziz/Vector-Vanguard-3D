#pragma once
#include "Component.h"
#include "raylib.h"
#include <nlohmann/json.hpp>

class TankController : public Component {
private:
    float moveSpeed = 5.0f;
    float rotationSpeed = 120.0f;

public:
    TankController(GameObject* owner) : Component(owner) {}
    static Component* CreateComponent(void* pData, GameObject* pOwner);
    void Update(float dt) override;
    void Render() override {}
    void SetMoveSpeed(float s) { moveSpeed = s; }
    void SetTurnSpeed(float s) { rotationSpeed = s; }
};