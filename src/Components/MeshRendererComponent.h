#pragma once
#include "Component.h"
#include "raylib.h"
#include <nlohmann/json.hpp>
#include "GameObject.h"

class MeshRendererComponent : public Component {
private:
    Model model = { 0 };
    Color color;
    bool hasModel = false;

public:
    Vector3 meshSize = { 1, 1, 1 };
    Vector3 meshOffset = { 0, 0, 0 };
    MeshRendererComponent(GameObject* owner)
        : Component(owner), color(WHITE), hasModel(false) {
    }
    ~MeshRendererComponent() {
        if (hasModel) {
            UnloadModel(model);
        }
    }
    // will parse "Shape", "Width", "Height", and "Color" from JSON
    static Component* CreateComponent(void* pData, GameObject* pOwner);

    void Update(float dt) override {}
    void Render() override;
    Model GetModel() const { return model; }
    bool HasModel() const { return hasModel; }
    void SetColor(Color newColor);
};