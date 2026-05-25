#include "AABBColliderComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "MeshRendererComponent.h" 
#include "raylib.h"
#include "raymath.h"

void AABBColliderComponent::Init(Vector3 localOffset, Vector3 boxSize) {
    this->offset = localOffset;
    this->size = boxSize;
    this->isInitialized = true;
}

void AABBColliderComponent::Update(float deltaTime) {
    // 1. If not initialized by JSON, try to find the Tank's mesh and measure it
    if (!isInitialized) {
        MeshRendererComponent* meshComp = owner->GetComponent<MeshRendererComponent>();
        if (meshComp && meshComp->HasModel()) {
            // Measure the actual model loaded in VRAM
            BoundingBox bb = GetMeshBoundingBox(meshComp->GetModel().meshes[0]);

            Vector3 meshSize = {
                bb.max.x - bb.min.x,
                bb.max.y - bb.min.y,
                bb.max.z - bb.min.z
            };

            Vector3 meshOffset = {
                (bb.max.x + bb.min.x) / 2.0f,
                (bb.max.y + bb.min.y) / 2.0f,
                (bb.max.z + bb.min.z) / 2.0f
            };

            Init(meshOffset, meshSize);
        }
        else return; // Wait for mesh to load
    }

    // 2. Standard World Space Update
    TransformComponent* t = owner->GetComponent<TransformComponent>();
    if (t) {
        Vector3 scaledSize = Vector3Multiply(size, t->scale);
        Vector3 trueCenter = Vector3Add(t->position, Vector3Multiply(offset, t->scale));
        Vector3 extents = Vector3Scale(scaledSize, 0.5f);

        minBounds = Vector3Subtract(trueCenter, extents);
        maxBounds = Vector3Add(trueCenter, extents);
    }
}

void AABBColliderComponent::Render() {
    if (!isInitialized) return;
    DrawBoundingBox({ minBounds, maxBounds }, GREEN);
}

bool AABBColliderComponent::Intersects(const AABBColliderComponent* other) const {
    if (!other || !this->isInitialized || !other->isInitialized) return false;

    return (this->minBounds.x <= other->maxBounds.x && this->maxBounds.x >= other->minBounds.x) &&
        (this->minBounds.y <= other->maxBounds.y && this->maxBounds.y >= other->minBounds.y) &&
        (this->minBounds.z <= other->maxBounds.z && this->maxBounds.z >= other->minBounds.z);
}

Component* AABBColliderComponent::CreateComponent(void* pData, GameObject* pOwner) {
    AABBColliderComponent* comp = new AABBColliderComponent(pOwner);

    nlohmann::json* j = static_cast<nlohmann::json*>(pData);
    // If "Size" is in JSON (like for Walls), Init immediately
    if (j && j->contains("Size")) {
        float sx = (*j)["Size"][0];
        float sy = (*j)["Size"][1];
        float sz = (*j)["Size"][2];
        comp->Init({ 0, 0, 0 }, { sx, sy, sz });
    }

    return comp;
}