//
// Created by berke on 4/5/2026.
//

#include "../../Headers/Objects/Player.h"

#include <iostream>


#include "../../Headers/Engine/InputManager.h"
#include "../../Headers/Engine/GameTime.h"
#include "../../Headers/Renderer/MapEditor.h"

#define FRICTION .8f
#define TURN_SPEED 2.0f


void Player::Update() {
    Vector2 input = {0.0f, 0.0f};

    if (InputManager::GetKey(SDL_SCANCODE_W)) input.y += 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_A)) input.x -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_S)) input.y -= 1.0f;
    if (InputManager::GetKey(SDL_SCANCODE_D)) input.x += 1.0f;

    if (InputManager::GetKey(SDL_SCANCODE_Q)) angle += TURN_SPEED * GameTime::deltaTime;
    if (InputManager::GetKey(SDL_SCANCODE_E)) angle -= TURN_SPEED * GameTime::deltaTime;

    angle += TURN_SPEED * GameTime::deltaTime * InputManager::GetMouseDelta().x;

    const Vector2 forward = {std::sin(angle), std::cos(angle)};
    const Vector2 right = {std::cos(angle), -std::sin(angle)};

    if (input.x != 0.0f || input.y != 0.0f) {
        const Vector2 moveDir = right * input.x + forward * input.y;
        velocity = moveDir.Normalized() * speed;
    } else {
        velocity *= FRICTION;
    }

    position += velocity * GameTime::deltaTime;
}
