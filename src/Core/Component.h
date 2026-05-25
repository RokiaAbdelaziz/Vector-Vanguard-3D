#pragma once
class GameObject;

class Component {
public:
    GameObject* owner;
    Component(GameObject* pOwner) : owner(pOwner) {}
    virtual ~Component() {}
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};