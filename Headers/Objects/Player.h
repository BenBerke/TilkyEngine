//
// Created by berke on 4/5/2026.
//

#ifndef TILKYENGINE_PLAYER_H
#define TILKYENGINE_PLAYER_H

#include "../Math/Vector.h"

class Player {
private:
    Vector2 velocity;

    Vector2 position;
    float speed;
    float size;
    float angle;

public:
    explicit Player(const Vector2 pos = {0, 0}, const float speed = 50.0f, const float size = 10.0f)
    : velocity(0.0f, 0.0f), position(pos), speed(speed), size(size) {}
    void Update();

    void SetVelocity(const Vector2 vel) { velocity = vel; }
    [[nodiscard]] Vector2 GetVelocity() { return velocity; }
    [[nodiscard]] Vector2 GetPosition() const { return position; }

    [[nodiscard]] float GetSize() const { return size; }
    [[nodiscard]] float GetAngle() const { return angle;}
};


#endif //TILKYENGINE_PLAYER_H