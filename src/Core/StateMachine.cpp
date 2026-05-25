#include "StateMachine.h"
#include "GameObject.h"
#include "EnemyStates.h" 
#include <nlohmann/json.hpp>

StateMachine::~StateMachine() {
    for (auto it = registeredStates.begin(); it != registeredStates.end(); ++it) {
        if (it->second) {
            delete it->second;
        }
    }
    registeredStates.clear();
}

void StateMachine::RegisterState(int stateId, State* stateInstance) {
    if (stateInstance) {
        // Ensure the state knows who its owner and machine are
        stateInstance->Init(this, owner);
        registeredStates[stateId] = stateInstance;
    }
}

void StateMachine::GoToState(int stateId) {
    //If already destroyed, stay destroyed forever
    if (currentState && currentStateId == EnemyStates::DESTROYED) {
        return;
    }

    // Check if the requested state ID exists in our map
    if (registeredStates.count(stateId)) {
        // Cleanup current state before leaving
        if (currentState) {
            currentState->Exit();
        }

        currentState = registeredStates[stateId];
        currentStateId = stateId; // Track the current ID for GetCurrentStateID()

        // Initialize the new state
        if (currentState) {
            currentState->Enter();
        }
    }
}

void StateMachine::Update(float deltaTime) {
    if (currentState) {
        currentState->Update(deltaTime);
    }
}

Component* StateMachine::CreateComponent(void* pData, GameObject* pOwner) {
    nlohmann::json* j = static_cast<nlohmann::json*>(pData);
    StateMachine* fsm = new StateMachine(pOwner);

    int initialID = j->value("InitialState", 0);

    fsm->currentStateId = initialID;

    return fsm;
}