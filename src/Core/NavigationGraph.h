#pragma once
#include "raylib.h"
#include <vector>
#include <cmath>

// Represents a single grid square / waypoint in the world 
class PathNode {
public:
    Vector3 position;
    int gridX, gridY;

    bool isStaticObstacle = false; // Walls 

    // A* specific data 
    float gCost = 0.0f; // Cost from start 
    float hCost = 0.0f; // Heuristic cost to goal 
    float fCost = 0.0f; // gCost + hCost 

    PathNode* parent = nullptr;

    // O(1) list checking optimization 
    bool inOpenList = false;
    bool inClosedList = false;

    PathNode(Vector3 pos, int x, int y) : position(pos), gridX(x), gridY(y) {}

    void Reset() {
        gCost = hCost = fCost = 0.0f;
        parent = nullptr;
        inOpenList = inClosedList = false;
    }

    // Determine if node is walkable 
    bool IsPassable() const { return !isStaticObstacle; }
};

class NavigationGraph {
private:
    std::vector<std::vector<PathNode*>> grid;
    int width, height;
    float cellSize;

    float CalculateHeuristic(PathNode* a, PathNode* b);
    std::vector<PathNode*> GetNeighbors(PathNode* node);

public:
    NavigationGraph(int gridWidth, int gridHeight, float cellSize);
    ~NavigationGraph();

    void SetStaticObstacle(int x, int y, bool isObstacle);
    PathNode* GetNodeFromWorldPosition(Vector3 worldPos);

    std::vector<PathNode*> FindPath(Vector3 startPos, Vector3 targetPos);
};