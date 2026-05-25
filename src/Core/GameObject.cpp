#include "GameObject.h"
#include "Component.h" 
#include <algorithm> 

GameObject::GameObject() : isActive(true), name("NewObject") {}

GameObject::~GameObject() {
  
    for (auto c : components) {
        delete c;
    }
    components.clear();
}

void GameObject::AddComponent(Component* component) {
    if (component) {
       
        component->owner = this;
        components.push_back(component);
    }
}

void GameObject::Update(float deltaTime) {
    if (!isActive) return;

    for (size_t i = 0; i < components.size(); ++i) {
        
        components[i]->Update(deltaTime);
    }
}

void GameObject::Render() {
    if (!isActive) return;

    for (auto c : components) {
        
        c->Render();
    }
}