//
// Created by berke on 4/5/2026.
//

#include "../../Headers/Objects/Player.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../../Headers/Engine/InputManager.h"
#include "../../Headers/Engine/GameTime.h"
#include "../../Headers/Renderer/MapEditor.h"

#define USE_MATH_DEFINES

#define FRICTION .8f
#define TURN_SPEED 90.0f
#define SENSITIVITY .5f

constexpr int COLLISION_ITERATIONS = 4;
constexpr float crouchSpeed = 10.0f;

static Vector2 ClosestPointOnSegment(const Wall& wall, const Vector2& p) {
    if (wall.lengthSq <= 0.00001f) {
        return wall.start;
    }

    float t = (p - wall.start).Dot(wall.dir) / wall.lengthSq;
    t = std::clamp(t, 0.0f, 1.0f);

    return wall.start + wall.dir * t;
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

    float targetHeight = eyeHeight;
    if (InputManager::GetKey(SDL_SCANCODE_C)) {
        targetHeight = eyeHeight * 0.5f;
    }

    const float newHeight =
        currentEyeHeight + (targetHeight - currentEyeHeight) * crouchSpeed * GameTime::deltaTime;

    SetCurrentEyeHeight(newHeight);

    float angleInRad = angle * M_PI / 180.0f;

    const float s = std::sin(angleInRad);
    const float c = std::cos(angleInRad);

    const Vector2 forward = {s, c};
    const Vector2 right = {c, -s};

    if (input.x != 0.0f || input.y != 0.0f) {
        const Vector2 moveDir = right * input.x + forward * input.y;
        velocity = moveDir.Normalized() * speed;
    } else {
        velocity *= FRICTION;
    }

    position += velocity * GameTime::deltaTime;
    position += tankInput * speed * GameTime::deltaTime;

    for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
        bool collided = false;

        for (const Wall& wall : walls) {
            const Vector2 closest = ClosestPointOnSegment(wall, position);
            const Vector2 delta = position - closest;

            const float distSq = delta.Dot(delta);
            const float radiusSq = size * size;

            if (distSq >= radiusSq) continue;

            float dist = std::sqrt(distSq);

            Vector2 normal;
            if (dist > 0.00001f) {
                normal = delta * (1.0f / dist);
            } else {
                normal = wall.normal;
            }

            const float penetration = size - dist;
            position += normal * penetration;

            const float intoWall = velocity.Dot(normal);
            if (intoWall < 0.0f) {
                velocity -= normal * intoWall;
            }

            collided = true;
        }

        if (!collided) break;
    }
}