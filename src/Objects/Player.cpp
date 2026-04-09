//
// Created by berke on 4/5/2026.
//

#include "../../Headers/Objects/Player.h"

#include <algorithm>
#include <cmath>

#include "../../Headers/Engine/InputManager.h"
#include "../../Headers/Engine/GameTime.h"
#include "../../Headers/Renderer/MapEditor.h"

#define USE_MATH_DEFINES

#define FRICTION .8f
#define TURN_SPEED 90.0f
#define SENSITIVITY .5f

constexpr int COLLISION_ITERATIONS = 4;
constexpr float crouchSpeed = 10.0f;
constexpr float stepSize = 10.0f;

static Vector2 ClosestPointOnSegment(const Wall& wall, const Vector2& p) {
    if (wall.lengthSq <= 0.00001f) {
        return wall.start;
    }

    float t = (p - wall.start).Dot(wall.dir) / wall.lengthSq;
    t = std::clamp(t, 0.0f, 1.0f);

    return wall.start + wall.dir * t;
}

void Player::Update(const std::vector<Wall>& walls, const std::vector<Sector>& sectors) {
    float targetHeight = eyeHeight;
    Vector2 input = {0.0f, 0.0f};
    Vector2 tankInput = {0.0f, 0.0f};

    if (InputManager::GetKey(SDL_SCANCODE_W)) input.y += 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_A)) input.x -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_S)) input.y -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_D)) input.x += 1.0f;

    if (InputManager::GetKey(SDL_SCANCODE_LSHIFT)) SetCurrentSpeed(GetSpeed() * 1.8f);
    else SetCurrentSpeed(GetSpeed());
    if (InputManager::GetKey(SDL_SCANCODE_C)) targetHeight *= 0.5f;

    angle += InputManager::GetMouseDelta().x * SENSITIVITY;

    targetHeight = sectors[GetCurrentSector()].floorHeight + GetEyeHeight();
    const float newHeight = currentEyeHeight + (targetHeight - currentEyeHeight) * crouchSpeed * GameTime::deltaTime;
    SetCurrentEyeHeight(newHeight);

    const float angleInRad = angle * M_PI / 180.0f;

    const float s = std::sin(angleInRad);
    const float c = std::cos(angleInRad);

    const Vector2 forward = {s, c};
    const Vector2 right = {c, -s};

    if (input.x != 0.0f || input.y != 0.0f) {
        const Vector2 moveDir = right * input.x + forward * input.y;
        velocity = moveDir.Normalized() * GetCurrentSpeed();
    } else {
        velocity *= FRICTION;
    }

    position += velocity * GameTime::deltaTime;
    position += tankInput * GetCurrentSpeed() * GameTime::deltaTime;

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
            //position += normal * penetration;

            const float intoWall = velocity.Dot(normal);
            if (intoWall < 0.0f) {
              //  velocity -= normal * intoWall;
            }

            collided = true;
        }

        if (!collided) break;
    }
}