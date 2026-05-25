#include "NavigationGraph.h"
#include "raymath.h"
#include <algorithm>

NavigationGraph::NavigationGraph(int gridWidth, int gridHeight, float cellSize)
    : width(gridWidth), height(gridHeight), cellSize(cellSize) {
    grid.resize(width, std::vector<PathNode*>(height));

    // Calculate the offset to center the logic grid
    float halfWidth = (width * cellSize) / 2.0f;
    float halfHeight = (height * cellSize) / 2.0f;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // Position nodes relative to the center (0,0,0). Node [0,0] will be at (-32, 0, -32)
            Vector3 pos = {
                ((float)x * cellSize) - halfWidth + (cellSize / 2.0f),
                0.0f,
                ((float)y * cellSize) - halfHeight + (cellSize / 2.0f)
            };
            grid[x][y] = new PathNode(pos, x, y);
        }
    }
}

NavigationGraph::~NavigationGraph() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            delete grid[x][y];
        }
    }
}

float NavigationGraph::CalculateHeuristic(PathNode* a, PathNode* b) {
    return (float)(abs(a->gridX - b->gridX) + abs(a->gridY - b->gridY));
}

std::vector<PathNode*> NavigationGraph::GetNeighbors(PathNode* node) {
    std::vector<PathNode*> neighbors;
    int x = node->gridX;
    int y = node->gridY;

    if (x > 0) neighbors.push_back(grid[x - 1][y]);
    if (x < width - 1) neighbors.push_back(grid[x + 1][y]);
    if (y > 0) neighbors.push_back(grid[x][y - 1]);
    if (y < height - 1) neighbors.push_back(grid[x][y + 1]);

    return neighbors;
}

void NavigationGraph::SetStaticObstacle(int x, int y, bool isObstacle) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        grid[x][y]->isStaticObstacle = isObstacle;
    }
}

PathNode* NavigationGraph::GetNodeFromWorldPosition(Vector3 worldPos) {
    float halfWidth = (width * cellSize) / 2.0f;
    float halfHeight = (height * cellSize) / 2.0f;

    // Convert world position back to grid index using the same offset
    int x = (int)floorf((worldPos.x + halfWidth) / cellSize);
    int y = (int)floorf((worldPos.z + halfHeight) / cellSize);

    if (x >= 0 && x < width && y >= 0 && y < height) return grid[x][y];
    return nullptr;
}

std::vector<PathNode*> NavigationGraph::FindPath(Vector3 startPos, Vector3 targetPos) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) grid[x][y]->Reset();
    }

    PathNode* startNode = GetNodeFromWorldPosition(startPos);
    PathNode* goalNode = GetNodeFromWorldPosition(targetPos);

    if (!startNode || !goalNode) return {};

    //if tank is brushing a wall, shift it to a neighbor
    if (!startNode->IsPassable()) {
        for (auto* n : GetNeighbors(startNode)) {
            if (n->IsPassable()) { startNode = n; break; }
        }
    }
    if (!goalNode->IsPassable()) {
        for (auto* n : GetNeighbors(goalNode)) {
            if (n->IsPassable()) { goalNode = n; break; }
        }
    }

    if (!startNode->IsPassable() || !goalNode->IsPassable() || startNode == goalNode) return {};

    std::vector<PathNode*> openList;
    startNode->gCost = 0;
    startNode->hCost = CalculateHeuristic(startNode, goalNode);
    startNode->fCost = startNode->gCost + startNode->hCost;
    startNode->inOpenList = true;
    openList.push_back(startNode);

    PathNode* current = nullptr;
    bool foundGoal = false;

    while (!openList.empty()) {
        auto it = std::min_element(openList.begin(), openList.end(), [](PathNode* a, PathNode* b) {
            return a->fCost < b->fCost;
            });

        current = *it;
        if (current == goalNode) {
            foundGoal = true;
            break;
        }

        openList.erase(it);
        current->inOpenList = false;
        current->inClosedList = true;

        for (PathNode* neighbor : GetNeighbors(current)) {
            if (!neighbor->IsPassable() || neighbor->inClosedList) continue;

            float tentativeGCost = current->gCost + 1.0f;

            if (!neighbor->inOpenList || tentativeGCost < neighbor->gCost) {
                neighbor->parent = current;
                neighbor->gCost = tentativeGCost;
                neighbor->hCost = CalculateHeuristic(neighbor, goalNode);
                neighbor->fCost = neighbor->gCost + neighbor->hCost;

                if (!neighbor->inOpenList) {
                    neighbor->inOpenList = true;
                    openList.push_back(neighbor);
                }
            }
        }
    }

    std::vector<PathNode*> path;
    if (foundGoal) {
        PathNode* temp = goalNode;
        while (temp != nullptr && temp != startNode) {
            path.push_back(temp);
            temp = temp->parent;
        }
        std::reverse(path.begin(), path.end());
    }

    return path;
}