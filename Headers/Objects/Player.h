//
// Created by berke on 4/5/2026.
//

#ifndef TILKYENGINE_PLAYER_H
#define TILKYENGINE_PLAYER_H

#include "../Math/Vector.h"
#include "../Renderer/MapEditor.h"

class Player {
private:
    Vector2 velocity;

    Vector2 position;
    float speed;
    float size;
    float angle;
    float eyeHeight;
    float stepSpeed;

    int currentSector{};
    float currentEyeHeight{};
    float currentSpeed{};

public:
    explicit Player(const Vector2 pos = {0, 0}, const float speed = 50.0f, const float size = 10.0f)
    : velocity(0.0f, 0.0f), position(pos), speed(speed), size(size), angle(0.0f), eyeHeight(32.0f), stepSpeed(150.0f), currentSector(-1), currentEyeHeight(eyeHeight)
    , currentSpeed(speed) {}
    void Update(const std::vector<Wall>& walls, const std::vector<Sector>& sectors);

    void SetVelocity(const Vector2 vel) { velocity = vel; }
    void SetEyeHeight(const float height) { eyeHeight = height; }
    void SetCurrentSector(const int sector) { currentSector = sector; }
    void SetCurrentEyeHeight(const float height) { currentEyeHeight = height; }
    void SetCurrentSpeed(const float s) { this->currentSpeed = s; }

    [[nodiscard]] Vector2 GetVelocity() const { return velocity; }
    [[nodiscard]] Vector2 GetPosition() const { return position; }
    [[nodiscard]] float GetSize() const { return size; }
    [[nodiscard]] float GetAngle() const { return angle;}
    [[nodiscard]] float GetEyeHeight() const { return eyeHeight; }
    [[nodiscard]] float GetStepSpeed() const { return stepSpeed; }
    [[nodiscard]] float GetSpeed() const { return speed; }

    [[nodiscard]] int GetCurrentSector() const { return currentSector; }
    [[nodiscard]] float GetCurrentEyeHeight() const { return currentEyeHeight; }
    [[nodiscard]] float GetCurrentSpeed() const { return currentSpeed; }
};

#endif //TILKYENGINE_PLAYER_H