#pragma once
#include <vector>
#include <string>
class Component;

class GameObject {
private:
    std::vector<Component*> components;
    bool isActive;
   

public:
    // Collision identification and lifecycle
    std::string name;
    bool isDead = false;
    bool isControllable = true;

    GameObject();
    ~GameObject();

    void Update(float deltaTime);
    void Render();

    // Adding components
    void AddComponent(Component* component);

    template <typename T>
    T* GetComponent() {
        for (auto c : components) {
            T* ptr = dynamic_cast<T*>(c);
            if (ptr) return ptr;
        }
        return nullptr;
    }

    void Destroy() { isDead = true; }
};