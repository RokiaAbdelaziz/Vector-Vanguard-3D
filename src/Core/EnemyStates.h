#pragma once

#include "GameObject.h"
#include "State.h"
#include "StateMachine.h"
#include "GameObjectManager.h"
#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "AABBColliderComponent.h"
#include "AINavigationComponent.h"
#include "ProjectileMovementComponent.h"
#include "raylib.h"
#include "raymath.h"
#include <iostream>

enum EnemyStates {
    CHASE = 0,
    DESTROYED = 1,
    ATTACK = 2
};

GameObject* GetClosestAliveTank(Vector3 myPosition, GameObject* me);

// ENEMY CHASE STATE 
class EnemyChaseState : public State {
private:
    float pathfindTimer = 0.0f;
    const float pathfindInterval = 0.1f;

public:
    void Enter() override {
        TraceLog(LOG_INFO, "Enemy Tank: Entering CHASE state.");
        pathfindTimer = pathfindInterval; // Force immediate pathfind on first Update
    }

    void Update(float dt) override {
        pathfindTimer += dt;

        TransformComponent* myTransform = owner->GetComponent<TransformComponent>();
        AINavigationComponent* nav = owner->GetComponent<AINavigationComponent>();

        if (!myTransform || !nav) return;

        // Periodic path refresh
        if (pathfindTimer >= pathfindInterval) {
            GameObject* target = GetClosestAliveTank(myTransform->position, owner);
            if (target) {
                Vector3 targetPos = target->GetComponent<TransformComponent>()->position;
                nav->CalculatePathTo(targetPos);
            }
            pathfindTimer = 0.0f;
        }

        // If close enough, ATTACK
        GameObject* target = GetClosestAliveTank(myTransform->position, owner);
        if (target) {
            float dist = Vector3Distance(myTransform->position, target->GetComponent<TransformComponent>()->position);
            if (dist < 12.0f) {
                stateMachine->GoToState(EnemyStates::ATTACK);
            }
        }
    }

    void Exit() override {}
};

//ENEMY ATTACK STATE 
class EnemyAttackState : public State {
private:
    float fireTimer = 0.0f;
    const float fireInterval = 1.5f;

public:
    void Enter() override {
        TraceLog(LOG_INFO, "Enemy Tank: Entering ATTACK state.");
        fireTimer = 0.7f;
    }

    void Update(float dt) override {
        fireTimer += dt;

        TransformComponent* myTransform = owner->GetComponent<TransformComponent>();
        if (!myTransform) return;

        GameObject* target = GetClosestAliveTank(myTransform->position, owner);
        if (!target) {
            stateMachine->GoToState(EnemyStates::CHASE);
            return;
        }

        Vector3 targetPos = target->GetComponent<TransformComponent>()->position;
        float dist = Vector3Distance(myTransform->position, targetPos);

        if (dist > 15.0f) {
            stateMachine->GoToState(EnemyStates::CHASE);
            return;
        }

        // Aim at target
        float dx = targetPos.x - myTransform->position.x;
        float dz = targetPos.z - myTransform->position.z;
        myTransform->rotation = { 0.0f, atan2f(dx, dz) * RAD2DEG, 0.0f };

        if (fireTimer >= fireInterval) {
            GameObject* bullet = GameObjectManager::Instance().CreateGameObject("Bullet.json");
            if (bullet) {
                TransformComponent* bTrans = bullet->GetComponent<TransformComponent>();
                if (bTrans) {
                    
                    float angleRad = myTransform->rotation.y * DEG2RAD;
                    Vector3 offset = {
                        sinf(angleRad) * 4.0f,
                        0.8f,
                        cosf(angleRad) * 4.0f
                    };

                    // Set the starting position and match the tank's rotation
                    bTrans->position = Vector3Add(myTransform->position, offset);
                    bTrans->rotation = myTransform->rotation;
                    
                    ProjectileMovementComponent* move = bullet->GetComponent<ProjectileMovementComponent>();
                    if (move) {
                        Vector3 forward = { sinf(angleRad), 0.0f, cosf(angleRad) };
                        move->SetDirection(forward);
                    }
                    
                }
            }
            fireTimer = 0.0f;
        }
    }

    void Exit() override {}
};

// ENEMY DESTROYED STATE 
class EnemyDestroyedState : public State {
public:
    void Enter() override {
        TraceLog(LOG_INFO, "Enemy Tank: DESTROYED.");
        AABBColliderComponent* collider = owner->GetComponent<AABBColliderComponent>();
        if (collider) collider->isInitialized = false;

        MeshRendererComponent* renderer = owner->GetComponent<MeshRendererComponent>();
        if (renderer) renderer->SetColor(DARKGRAY);
    }

    void Update(float deltaTime) override {}
    void Exit() override {}
};