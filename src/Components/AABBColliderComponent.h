#pragma once
#include "Component.h"
#include "raylib.h"
#include <nlohmann/json.hpp>
#include "GameObject.h"

class AABBColliderComponent : public Component {
private:
    // These define the shape based on the MeshRenderer's measurements
    Vector3 offset;
    Vector3 size;
    Vector3 minBounds;
    Vector3 maxBounds;

public:
    bool isInitialized = false;
    AABBColliderComponent(GameObject* owner) : Component(owner),
        offset({ 0,0,0 }), size({ 0,0,0 }), minBounds({ 0,0,0 }), maxBounds({ 0,0,0 }), isInitialized(false) {
    }

    ~AABBColliderComponent() override = default;
    void Init(Vector3 localOffset, Vector3 boxSize);
    void Update(float deltaTime) override;
    //  "Hitbox" for debugging
    void Render() override;
    bool Intersects(const AABBColliderComponent* other) const;
    static Component* CreateComponent(void* pData, GameObject* pOwner);
    Vector3 GetMin() const { return minBounds; }
    Vector3 GetMax() const { return maxBounds; }
    bool IsReady() const { return isInitialized; }
};