#pragma once
#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "raylib.h"
#include "rlgl.h"

class TankRenderer : public Component {
public:
    void Update(float dt) override {}

    void Render() override {
        TransformComponent* t = owner->GetComponent<TransformComponent>();
        if (t) {
            rlPushMatrix();
            // Move the "drawing cursor" to the tank's position
            rlTranslatef(t->position.x, t->position.y, t->position.z);
            // Rotate the "drawing cursor" around the Y axis
            rlRotatef(t->rotation.y, 0, 1, 0);
            DrawCubeWires({ 0, 0, 0 }, 2.0f, 1.0f, 3.0f, LIME);
            Vector3 turretOffset = { 0, 0.75f, 0 };
            DrawCubeWires(turretOffset, 1.0f, 0.5f, 1.0f, GREEN);
            rlPopMatrix();
        }
    }
};