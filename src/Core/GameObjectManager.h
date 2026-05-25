#pragma once
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp> 
#include "GameObject.h"
using json = nlohmann::json;
typedef Component* (*ComponentCreator)(void* pDataNode, GameObject* pOwner);

class GameObjectManager {
private:
    std::map<std::string, ComponentCreator> m_mComponentCreators;

    // list of all active GameObjects
    std::vector<GameObject*> gameObjects;
    GameObjectManager() {}
    GameObjectManager(const GameObjectManager&) = delete;
    GameObjectManager& operator=(const GameObjectManager&) = delete;

public:
    static GameObjectManager& Instance() {
        static GameObjectManager instance;
        return instance;
    }
    void RegisterComponentFactory(const std::string& typeName, ComponentCreator pFunc);
    GameObject* CreateGameObject(const std::string& filePath);
    void Update(float deltaTime);
    void Render();
    const std::vector<GameObject*>& GetGameObjects() const { return gameObjects; }
    void Shutdown();
};