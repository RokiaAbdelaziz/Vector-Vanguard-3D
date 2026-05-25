#include "ProjectileMovementComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "raymath.h"

Component* ProjectileMovementComponent::CreateComponent(void* pData, GameObject* pOwner) {
    nlohmann::json* j = static_cast<nlohmann::json*>(pData);
    ProjectileMovementComponent* comp = new ProjectileMovementComponent(pOwner);
    comp->speed = j->value("Speed", 20.0f);
    comp->lifetime = j->value("Lifetime", 3.0f);

    return comp;
}

void ProjectileMovementComponent::Update(float deltaTime) {
    timer += deltaTime;
    if (timer >= lifetime) {
        owner->Destroy(); 
        return;
    }
    TransformComponent* t = owner->GetComponent<TransformComponent>();
    if (t && (direction.x != 0 || direction.y != 0 || direction.z != 0)) {
        Vector3 velocity = Vector3Scale(direction, speed * deltaTime);
        t->position.x += velocity.x;
        t->position.y += velocity.y;
        t->position.z += velocity.z;
    }
}