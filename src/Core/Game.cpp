#include "Game.h"
#include "GameObjectManager.h"
#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "AABBColliderComponent.h"
#include "TankControllerComponent.h"
#include "ProjectileMovementComponent.h"
#include "StateMachine.h"
#include "AINavigationComponent.h" 
#include "NavigationGraph.h" 
#include "EnemyStates.h"
#include <fstream>
#include <nlohmann/json.hpp>

// find the nearest target for the AI
GameObject* GetClosestAliveTank(Vector3 myPosition, GameObject* me) {
    GameObject* closestTarget = nullptr;
    float closestDistance = 9999.0f;

    auto& objects = GameObjectManager::Instance().GetGameObjects();
    for (auto* obj : objects) {
        if (obj == me || obj->name == "Wall" || obj->name == "PlasmaBullet") continue;

        StateMachine* fsm = obj->GetComponent<StateMachine>();
        if (fsm && fsm->GetCurrentStateID() == EnemyStates::DESTROYED) continue;

        TransformComponent* trans = obj->GetComponent<TransformComponent>();
        if (trans) {
            float dist = Vector3Distance(myPosition, trans->position);
            if (dist < closestDistance) {
                closestDistance = dist;
                closestTarget = obj;
            }
        }
    }
    return closestTarget;
}

Game::Game(int width, int height, std::string title) {
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    auto& gom = GameObjectManager::Instance();
    gom.RegisterComponentFactory("TransformComponent", TransformComponent::CreateComponent);
    gom.RegisterComponentFactory("MeshRendererComponent", MeshRendererComponent::CreateComponent);
    gom.RegisterComponentFactory("AABBColliderComponent", AABBColliderComponent::CreateComponent);
    gom.RegisterComponentFactory("TankControllerComponent", TankController::CreateComponent);
    gom.RegisterComponentFactory("ProjectileMovementComponent", ProjectileMovementComponent::CreateComponent);
    gom.RegisterComponentFactory("StateMachineComponent", StateMachine::CreateComponent);
    gom.RegisterComponentFactory("AINavigationComponent", AINavigationComponent::CreateComponent);

    Initialize();
    isRunning = true;
}

Game::~Game() {
    Shutdown();
    if (navGraph) delete navGraph;
}

void Game::Initialize() {
    //  the navigation grid (16x16 with 4.0 unit cells)
    navGraph = new NavigationGraph(16, 16, 4.0f);

    std::ifstream file("Level.json");
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open Level.json!");
        return;
    }
    nlohmann::json levelData;
    file >> levelData;

    auto grid = levelData["Grid"];
    float cellSize = levelData["CellSize"];

    float halfWidth = (16 * cellSize) / 2.0f;
    float halfHeight = (16 * cellSize) / 2.0f;

    //  Parse the grid and spawn objects
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            int type = grid[y][x];
            Vector3 pos = {
                ((float)x * cellSize) - halfWidth + (cellSize / 2.0f),
                0.0f,
                ((float)y * cellSize) - halfHeight + (cellSize / 2.0f)
            };

            switch (type) {
            case 1: { // Wall
                GameObject* wall = GameObjectManager::Instance().CreateGameObject("Wall.json");
                if (wall) {
                    wall->GetComponent<TransformComponent>()->position = pos;
                    navGraph->SetStaticObstacle(x, y, true);
                }
                break;
            }
            case 2: { // Enemy
                GameObject* enemy = GameObjectManager::Instance().CreateGameObject("EnemyTank.json");
                if (enemy) {
                    enemy->GetComponent<TransformComponent>()->position = pos;

                    StateMachine* fsm = enemy->GetComponent<StateMachine>();
                    AINavigationComponent* nav = enemy->GetComponent<AINavigationComponent>();

                    if (nav) nav->SetNavigationGraph(navGraph);

                    if (fsm) {
                        fsm->RegisterState(EnemyStates::CHASE, new EnemyChaseState());
                        fsm->RegisterState(EnemyStates::DESTROYED, new EnemyDestroyedState());
                        fsm->RegisterState(EnemyStates::ATTACK, new EnemyAttackState());
                        fsm->GoToState(EnemyStates::CHASE);

                        GameObject* target = GetClosestAliveTank(pos, enemy);
                        if (target && nav) {
                            nav->CalculatePathTo(target->GetComponent<TransformComponent>()->position);
                        }
                    }
                }
                break;
            }
            case 3: { // Player
                playerTank = GameObjectManager::Instance().CreateGameObject("PlayerTank.json");
                if (playerTank) {
                    playerTank->GetComponent<TransformComponent>()->position = pos;
                }
                break;
            }
            default: break;
            }
        }
    }
    camera.position = { 0.0f, 45.0f, 60.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void Game::Update(float dt) {
    GameObjectManager::Instance().Update(dt);

    auto& objects = GameObjectManager::Instance().GetGameObjects();
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            GameObject* objA = objects[i];
            GameObject* objB = objects[j];

            // Skip if either object is already marked for destruction
            if (objA->isDead || objB->isDead) continue;

            auto* colA = objA->GetComponent<AABBColliderComponent>();
            auto* colB = objB->GetComponent<AABBColliderComponent>();

            if (colA && colB && colA->Intersects(colB)) {

                // BULLET COLLISIONS
              
                bool aIsBullet = (objA->name == "PlasmaBullet" || objA->name == "Bullet");
                bool bIsBullet = (objB->name == "PlasmaBullet" || objB->name == "Bullet");

                if (aIsBullet || bIsBullet) {
                    GameObject* bullet = aIsBullet ? objA : objB;
                    GameObject* other = aIsBullet ? objB : objA;

                    // Ensure we aren't hitting another bullet
                    if (other->name != "Bullet") {

                        // Check if the 'other' is a Tank (Player or Enemy)
                        if (other == playerTank || other->name == "PlayerTank" || other->name == "Player") {
                            TraceLog(LOG_INFO, "PLAYER HIT!!!....LOST");

                            auto* renderer = other->GetComponent<MeshRendererComponent>();
                            if (renderer) renderer->SetColor(GRAY);

                            other->isControllable = false;
                            bullet->Destroy(); // Make the bullet disappear
                        }
                        // Handle Enemy Hit (FSM Destroyed State)
                        else if (other->name == "EnemyTank") {
                            StateMachine* fsm = other->GetComponent<StateMachine>();
                            if (fsm) fsm->GoToState(EnemyStates::DESTROYED);
                        }

                        bullet->Destroy();
                    }

                }
                // PHYSICAL (NON-BULLET) COLLISIONS
                else {
                    // A. Check for Tank vs. Tank (Player vs. Enemy)
                    bool aIsTank = (objA->name == "PlayerTank" || objA->name == "EnemyTank" || objA->name == "Player");
                    bool bIsTank = (objB->name == "PlayerTank" || objB->name == "EnemyTank" || objB->name == "Player");

                    if (aIsTank && bIsTank) {
                        TransformComponent* tA = objA->GetComponent<TransformComponent>();
                        TransformComponent* tB = objB->GetComponent<TransformComponent>();
                        if (tA && tB) {
                            // Push both tanks away from each other
                            Vector3 pushDir = Vector3Normalize(Vector3Subtract(tA->position, tB->position));
                            tA->Translate(Vector3Scale(pushDir, 0.1f));
                            tB->Translate(Vector3Scale(pushDir, -0.1f));
                        }
                    }

                    // TANK VS WALL COLLISIONS 
                    else {
                        GameObject* tank = nullptr;
                        GameObject* wall = nullptr;

                        // Helper lambda to identify Tank vs Wall
                        auto isTank = [](GameObject* g) { return g->name == "PlayerTank"; };
                        auto isWall = [](GameObject* g) { return g->name == "Wall"; };

                        if (isTank(objA) && isWall(objB)) { tank = objA; wall = objB; }
                        else if (isTank(objB) && isWall(objA)) { tank = objB; wall = objA; }

                        if (tank && wall) {
                            TransformComponent* tTrans = tank->GetComponent<TransformComponent>();
                            TransformComponent* wTrans = wall->GetComponent<TransformComponent>();
                            if (tTrans && wTrans) {
                                Vector3 pushDir = Vector3Normalize(Vector3Subtract(tTrans->position, wTrans->position));
                                tTrans->Translate(Vector3Scale(pushDir, 0.2f));
                            }
                        }
                    }
                }
            }
        }
    }
}

void Game::Render() {
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);
    DrawGrid(16, 4.0f); 
    GameObjectManager::Instance().Render();
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void Game::Shutdown() {
    GameObjectManager::Instance().Shutdown();
    if (IsWindowReady()) CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose() && isRunning) {
        Update(GetFrameTime());
        Render();
    }
}