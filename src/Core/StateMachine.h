#pragma once
#include <map>
#include <nlohmann/json.hpp>
#include "Component.h"
#include "State.h"

class GameObject;

class StateMachine : public Component {
private:
    // Map of IDs to State instances
    std::map<int, State*> registeredStates;
    State* currentState = nullptr;
    int currentStateId = -1;

public:
    StateMachine(GameObject* owner) : Component(owner), currentState(nullptr) {}
    ~StateMachine() override;

    //  to filter targets
    int GetCurrentStateID() const { return currentStateId; }

    // Register a new state with an ID
    void RegisterState(int stateId, State* stateInstance);

    // Transition to a new state
    void GoToState(int stateId);

    void Update(float deltaTime) override;
    void Render() override {}

    // Factory method for GameObjectManager
    static Component* CreateComponent(void* pData, GameObject* pOwner);
};