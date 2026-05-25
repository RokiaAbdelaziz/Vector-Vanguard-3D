#include "MeshRendererComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "AABBColliderComponent.h"
#include "raylib.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Component* MeshRendererComponent::CreateComponent(void* pData, GameObject* pOwner) {
    json* j = static_cast<json*>(pData);

    Mesh mesh = { 0 };
    std::string shape = (*j).value("Shape", "Cube");

    // Generate Mesh based on JSON Shape
    if (shape == "Cube") {
        float w = (*j).value("Width", 1.0f);
        float h = (*j).value("Height", 1.0f);
        float l = (*j).value("Length", 1.0f);
        mesh = GenMeshCube(w, h, l);
    }
    else if (shape == "Sphere") {
        float r = (*j).value("Radius", 1.0f);
        mesh = GenMeshSphere(r, 16, 16);
    }

    // Wrap Mesh into a Model
    Model model = LoadModelFromMesh(mesh);

    MeshRendererComponent* renderer = new MeshRendererComponent(pOwner);
    renderer->model = model;
    renderer->hasModel = true;
    if (j->contains("Color") && (*j)["Color"].is_array()) {
        renderer->color.r = (unsigned char)(*j)["Color"][0];
        renderer->color.g = (unsigned char)(*j)["Color"][1];
        renderer->color.b = (unsigned char)(*j)["Color"][2];
        renderer->color.a = (unsigned char)(*j)["Color"][3];
    }
    // automatic bounding box from the generated mesh
    BoundingBox localBounds = GetMeshBoundingBox(mesh);

    Vector3 size = {
        localBounds.max.x - localBounds.min.x,
        localBounds.max.y - localBounds.min.y,
        localBounds.max.z - localBounds.min.z
    };

    Vector3 offset = {
        (localBounds.max.x + localBounds.min.x) / 2.0f,
        (localBounds.max.y + localBounds.min.y) / 2.0f,
        (localBounds.max.z + localBounds.min.z) / 2.0f
    };

    renderer->meshSize = size;
    renderer->meshOffset = offset;

    return renderer;
}

void MeshRendererComponent::Render() {
    TransformComponent* t = owner->GetComponent<TransformComponent>();

    if (t && hasModel) {
        // DrawModelEx handles Position, Rotation (around Y axis), and Scale
        DrawModelEx(model, t->position, { 0, 1, 0 }, t->rotation.y, t->scale, color);
    }
}
void MeshRendererComponent::SetColor(Color newColor) {
    color = newColor;
}