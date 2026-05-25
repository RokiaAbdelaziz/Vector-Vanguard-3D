#pragma once
#include "Component.h"
#include "raylib.h"
#include <nlohmann/json.hpp>

class GameObject;

class TransformComponent : public Component {
public:
    Vector3 position = { 0, 0, 0 };
    Vector3 rotation = { 0, 0, 0 };
    Vector3 scale = { 1, 1, 1 };

    TransformComponent(GameObject* owner) : Component(owner) {}

    static Component* CreateComponent(void* pData, GameObject* pOwner) {
        nlohmann::json* j = static_cast<nlohmann::json*>(pData);
        TransformComponent* comp = new TransformComponent(pOwner);

        // Position [x, y, z] from JSON
        if (j->contains("Position") && (*j)["Position"].is_array()) {
            comp->position.x = (*j)["Position"][0];
            comp->position.y = (*j)["Position"][1];
            comp->position.z = (*j)["Position"][2];
        }

        // Rotation [x, y, z] from JSON
        if (j->contains("Rotation") && (*j)["Rotation"].is_array()) {
            comp->rotation.x = (*j)["Rotation"][0];
            comp->rotation.y = (*j)["Rotation"][1];
            comp->rotation.z = (*j)["Rotation"][2];
        }

        // Scale [x, y, z] from JSON
        if (j->contains("Scale") && (*j)["Scale"].is_array()) {
            comp->scale.x = (*j)["Scale"][0];
            comp->scale.y = (*j)["Scale"][1];
            comp->scale.z = (*j)["Scale"][2];
        }

        return comp;
    }

    // Moving the object
    void Translate(Vector3 translation) {
        position.x += translation.x;
        position.y += translation.y;
        position.z += translation.z;
    }

    void Update(float dt) override {}
    void Render() override {}
};