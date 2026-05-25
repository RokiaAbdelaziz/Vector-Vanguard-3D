#include "raylib.h"           
#include "GameObjectManager.h"
#include "GameObject.h" 
#include <fstream>
#include <iostream>

void GameObjectManager::RegisterComponentFactory(const std::string& typeName, ComponentCreator pFunc) {
    m_mComponentCreators[typeName] = pFunc;
}

GameObject* GameObjectManager::CreateGameObject(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open JSON file: %s", filePath.c_str());
        return nullptr;
    }

    json data;
    try {
        file >> data;
    }
    catch (json::parse_error& e) {
        TraceLog(LOG_ERROR, "JSON Parse Error in %s: %s", filePath.c_str(), e.what());
        return nullptr;
    }

    GameObject* newObj = new GameObject();

    if (data.contains("Name")) {
        newObj->name = data["Name"];
    }
    else if (data.contains("ObjectName")) {
        newObj->name = data["ObjectName"];
    }

    if (data.contains("Components")) {
        auto& componentsNode = data["Components"];

        if (componentsNode.is_array()) {
            for (auto& compNode : componentsNode) {
                std::string type = compNode.value("Type", "");
                if (m_mComponentCreators.count(type)) {
                    Component* newComp = m_mComponentCreators[type](&compNode, newObj);
                    if (newComp) newObj->AddComponent(newComp);
                }
                else {
                    TraceLog(LOG_WARNING, "Component type '%s' not registered", type.c_str());
                }
            }
        }
        else if (componentsNode.is_object()) {
            for (auto& [type, config] : componentsNode.items()) {
                if (m_mComponentCreators.count(type)) {
                    // Pass the config block to the factory
                    Component* newComp = m_mComponentCreators[type](&config, newObj);
                    if (newComp) newObj->AddComponent(newComp);
                }
                else {
                    TraceLog(LOG_WARNING, "Component type '%s' not registered", type.c_str());
                }
            }
        }
    }

    gameObjects.push_back(newObj);
    return newObj;
}

void GameObjectManager::Update(float dt) {
    for (size_t i = 0; i < gameObjects.size(); i++) {
        if (gameObjects[i] != nullptr && !gameObjects[i]->isDead) {
            gameObjects[i]->Update(dt);
        }
    }


    for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
        if ((*it)->isDead) {
            delete* it;
            it = gameObjects.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GameObjectManager::Render() {
    for (auto* obj : gameObjects) {
        if (obj && !obj->isDead) {
            obj->Render();
        }
    }
}

void GameObjectManager::Shutdown() {
    for (auto* obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}