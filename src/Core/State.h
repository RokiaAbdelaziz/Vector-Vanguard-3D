#pragma once
class GameObject;
class StateMachine;

class State {
protected:
    StateMachine* stateMachine = nullptr;
    GameObject* owner = nullptr;

public:
    virtual ~State() = default;
    void Init(StateMachine* sm, GameObject* obj) {
        stateMachine = sm;
        owner = obj;
    }
    virtual void Enter() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Exit() = 0;
};