#include "AINavigationComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "raymath.h"
#include <nlohmann/json.hpp>

Component* AINavigationComponent::CreateComponent(void* pData, GameObject* pOwner) {
    nlohmann::json* j = static_cast<nlohmann::json*>(pData);
    AINavigationComponent* comp = new AINavigationComponent(pOwner);

    // Load moveSpeed from JSON, default to 5.0f if not found
    comp->moveSpeed = j->value("MoveSpeed", 5.0f);

    return comp;
}

void AINavigationComponent::CalculatePathTo(Vector3 targetWorldPos) {
    if (!navGraph) return;

    TransformComponent* transform = owner->GetComponent<TransformComponent>();
    if (transform) {
        // FORCE Y TO ZERO: Ensure 3D height doesn't break 2D grid math
        Vector3 flatStart = { transform->position.x, 0.0f, transform->position.z };
        Vector3 flatTarget = { targetWorldPos.x, 0.0f, targetWorldPos.z };

        std::vector<PathNode*> newPath = navGraph->FindPath(flatStart, flatTarget);

        if (!newPath.empty()) {
            currentPath = newPath;
            currentWaypointIndex = 0;
        }
    }
}

void AINavigationComponent::Update(float deltaTime) {
    // If currentPath is empty or we reached the end, do nothing
    if (currentPath.empty() || currentWaypointIndex >= (int)currentPath.size()) return;

    TransformComponent* transform = owner->GetComponent<TransformComponent>();
    if (!transform) return;

    // Get the PathNode position at the currentWaypointIndex
    Vector3 targetPos = currentPath[currentWaypointIndex]->position;
    Vector3 currentPos = transform->position;

    // 1. Movement Logic: Calculate exact direction and move
    Vector3 dir = Vector3Normalize(Vector3Subtract(targetPos, currentPos));

    // Safety check: ensure speed is never 0
    float actualSpeed = (moveSpeed > 0) ? moveSpeed : 5.0f;
    transform->position = Vector3Add(currentPos, Vector3Scale(dir, actualSpeed * deltaTime));

    // 2. Snapping & Hard Turning: Check distance to waypoint
    if (Vector3Distance(transform->position, targetPos) < 0.3f) {

        // Step 1: Snap Position to target node center
        transform->position = targetPos;

        // Step 2: Increment waypoint index
        currentWaypointIndex++;

        // Step 3: Hard Turn (If there is a next node)
        if (currentWaypointIndex < (int)currentPath.size()) {
            Vector3 nextNodePos = currentPath[currentWaypointIndex]->position;

            float dx = nextNodePos.x - transform->position.x;
            float dz = nextNodePos.z - transform->position.z;

            // Calculate angle in radians
            float angleRadians = atan2f(dx, dz);

            // Set rotation in degrees around the Y-axis (Yaw)
            transform->rotation = { 0.0f, angleRadians * RAD2DEG, 0.0f };
        }
    }
}

void AINavigationComponent::Render() {
    if (currentPath.empty()) return;

    // Lift the lines 0.5 units off the ground so they are clearly visible
    Vector3 yOffset = { 0.0f, 0.5f, 0.0f };
    Vector3 currentPos = owner->GetComponent<TransformComponent>()->position;

    // Draw line from tank to first waypoint
    if (currentWaypointIndex < (int)currentPath.size()) {
        DrawLine3D(Vector3Add(currentPos, yOffset), 
                   Vector3Add(currentPath[currentWaypointIndex]->position, yOffset), GREEN);
    }

    // Draw lines between all remaining waypoints
    for (int i = currentWaypointIndex; i < (int)currentPath.size() - 1; i++) {
        DrawLine3D(Vector3Add(currentPath[i]->position, yOffset), 
                   Vector3Add(currentPath[i + 1]->position, yOffset), GREEN);
    }
}