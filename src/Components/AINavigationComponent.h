#pragma once
#include "Component.h"
#include "NavigationGraph.h"
#include "TransformComponent.h"
#include "raylib.h"
#include <vector>

// Forward declaration to prevent circular includes
class NavigationGraph;
struct PathNode;

class AINavigationComponent : public Component {
public:
    // Pointers and Path Data
    NavigationGraph* navGraph = nullptr;
    std::vector<PathNode*> currentPath;
    int currentWaypointIndex = 0;
    float moveSpeed = 5.0f;

    // Constructor
    AINavigationComponent(GameObject* owner) : Component(owner) {}

    // Factory method for JSON loading
    static Component* CreateComponent(void* pData, GameObject* pOwner);

    // Setters
    void SetNavigationGraph(NavigationGraph* graph) { navGraph = graph; }

    // Core logic
    void CalculatePathTo(Vector3 targetWorldPos);

    // Engine overrides
    void Update(float deltaTime) override;
    void Render() override; 
};