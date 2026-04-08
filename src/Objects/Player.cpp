//
// Created by berke on 4/5/2026.
//

#include "../../Headers/Objects/Player.h"

#include <algorithm>
#include <iostream>

#include "../../Headers/Engine/InputManager.h"
#include "../../Headers/Engine/GameTime.h"
#include "../../Headers/Renderer/MapEditor.h"

#define USE_MATH_DEFINES

#define FRICTION .8f
#define TURN_SPEED 90.0f
#define SENSITIVITY .5f

constexpr int COLLISION_ITERATIONS = 4;

Vector2 ClosestPointOnSegmet(const Vector2& a, const Vector2& b, const Vector2& p) {
    Vector2 ab = b - a;
    const float abLenSq = ab.Dot(ab);
    if (abLenSq <= 0.00001f) return a;

    float t = (p - a).Dot(ab) / abLenSq;
    t = std::clamp(t, 0.0f, 1.0f);

    return a + ab * t;
}

void Player::Update(std::vector<Wall>& walls) {
    Vector2 input = {0.0f, 0.0f};
    Vector2 tankInput = {0.0f, 0.0f};

    if (InputManager::GetKey(SDL_SCANCODE_W)) input.y += 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_A)) input.x -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_S)) input.y -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_D)) input.x += 1.0f;

    if (InputManager::GetKey(SDL_SCANCODE_UP)) tankInput.y += 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_LEFT)) tankInput.x -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_DOWN)) tankInput.y -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_RIGHT)) tankInput.x += 1.0f;

    if (InputManager::GetKey(SDL_SCANCODE_Q)) angle -= TURN_SPEED * GameTime::deltaTime;
    if (InputManager::GetKey(SDL_SCANCODE_E)) angle += TURN_SPEED * GameTime::deltaTime;

    angle += InputManager::GetMouseDelta().x * SENSITIVITY;

    float angleInRad = angle * M_PI / 180.0f;

    const float sin = std::sin(angleInRad);
    const float cos = std::cos(angleInRad);

    const Vector2 forward = {sin, cos};
    const Vector2 right = {cos, -sin};

    if (input.x != 0.0f || input.y != 0.0f) {
        const Vector2 moveDir = right * input.x + forward * input.y;
        velocity = moveDir.Normalized() * speed;
    } else velocity *= FRICTION;

    position += velocity * GameTime::deltaTime;
    position += tankInput * speed * GameTime::deltaTime;

    for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
        bool collider = false;
        for (const Wall& wall : walls) {
            const Vector2 closest = ClosestPointOnSegmet(wall.start, wall.end, position);
            const Vector2 delta = position - closest;

            const float distSq = delta.Dot(delta);
            const float radiusSq = size * size;
            if (distSq > radiusSq) continue;

            float dist = std::sqrt(distSq);

            Vector2 normal;
            if (dist > 0.00001f) normal = delta * (1.0/dist);
            else {
                Vector2 wallDir = wall.end - wall.start;
                normal = {-wallDir.y, wallDir.x};
                normal.Normalize();
            }

            float penetration = size - dist;
            position += normal * penetration;
        }
    }
}

