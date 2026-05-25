#include "TankControllerComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GameObjectManager.h"
#include "ProjectileMovementComponent.h"
#include "EventManager.h"
#include "GameEvents.h"
#include <cmath>
Component* TankController::CreateComponent(void* pData, GameObject* pOwner) {
    nlohmann::json* j = static_cast<nlohmann::json*>(pData);
    TankController* comp = new TankController(pOwner);

    comp->SetMoveSpeed((*j).value("MoveSpeed", 10.0f));
    comp->SetTurnSpeed((*j).value("TurnSpeed", 120.0f));

    return comp;
}

void TankController::Update(float dt) {
    TransformComponent* t = owner->GetComponent<TransformComponent>();
    if (!owner->isControllable) {
        return; // Skip all keyboard checks if the tank is frozen
    }
    if (!t) return;

    // Rotation (A/D)
    if (IsKeyDown(KEY_A)) t->rotation.y += rotationSpeed * dt;
    if (IsKeyDown(KEY_D)) t->rotation.y -= rotationSpeed * dt;

    // Calculate Forward Vector
    float radians = t->rotation.y * DEG2RAD;
    Vector3 forward = { sinf(radians), 0.0f, cosf(radians) };

    // Movement (W/S)
    if (IsKeyDown(KEY_W)) {
        t->position.x += forward.x * moveSpeed * dt;
        t->position.z += forward.z * moveSpeed * dt;
    }
    if (IsKeyDown(KEY_S)) {
        t->position.x -= forward.x * moveSpeed * dt;
        t->position.z -= forward.z * moveSpeed * dt;
    }

    // Spawning the Bullet
    if (IsKeyPressed(KEY_SPACE)) {
        // Create the bullet from the JSON
        GameObject* bullet = GameObjectManager::Instance().CreateGameObject("Bullet.json");

        if (bullet) {
            TransformComponent* bulletT = bullet->GetComponent<TransformComponent>();
            if (bulletT) {
                // Override position and rotation to match the tank's front
                bulletT->position = t->position;
                bulletT->rotation = t->rotation;

                // Offset the bullet slightly so it doesn't spawn inside the tank
                bulletT->position.x += forward.x * 2.0f;
                bulletT->position.z += forward.z * 2.0f;
            }
            // Tell the ProjectileMovementComponent which way to go
            ProjectileMovementComponent* move = bullet->GetComponent<ProjectileMovementComponent>();
            if (move) {
                move->SetDirection(forward); // Pass the tank's current forward vector
            }
            WeaponFiredEvent e(t->position, 100.0f);
            EventManager::Instance().TriggerEvent(e);
        }
    }
}