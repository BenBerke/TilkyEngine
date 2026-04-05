//
// Created by berke on 4/5/2026.
//

#ifndef TILKYENGINE_PLAYER_H
#define TILKYENGINE_PLAYER_H

#include "Vector.h"
#include "GameTime.h"

#define FRICTION .8f

class Player {
private:
    Vector2 velocity;

    Vector2 position;
    float speed;
    float size;

public:
    explicit Player(const Vector2 pos = {0, 0}, const float speed = 50.0f, const float size = 10.0f)
    : velocity(0.0f, 0.0f), position(pos), speed(speed), size(size) {}
    void Update() {
        position += velocity.Normalized() * GameTime::deltaTime * speed;
        velocity *= FRICTION;
    }

    void SetVelocity(const Vector2 vel) { velocity = vel; }
    [[nodiscard]] Vector2 GetVelocity() { return velocity; }
    [[nodiscard]] Vector2 GetPosition() const { return position; }
};


#endif //TILKYENGINE_PLAYER_H