#pragma once
#include "raylib.h"
#include <string>
#include "NavigationGraph.h" 

class GameObject;

class Game {
public:
    Game(int width, int height, std::string title);
    ~Game();
    void Run();

private:
    void Initialize();
    void Update(float deltaTime);
    void Render();
    void Shutdown();

    Camera3D camera;
    bool isRunning;
    GameObject* playerTank;

    NavigationGraph* navGraph = nullptr;

    Vector3 lastFirePos = { 0, -100, 0 };
    bool showFireSphere = false;
};