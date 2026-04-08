//
// Created by berke on 4/8/2026.
//

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>

#include <string>

#include "../../Headers/Renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "../../Headers/Engine/GameTime.h"

#define USE_MATH_DEFINES

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

#define FPS_COOLDOWN_SECONDS 1.0f

#define DEBUG 1

namespace {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
}

namespace Renderer {
    bool Initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
            return false;
        }
        if (SDL_CreateWindowAndRenderer("Tilky Engine", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) == false) {
            SDL_Log("Window/Renderer Error: %s\n", SDL_GetError());
            SDL_Quit();
            return false;
        }
        SDL_SetWindowRelativeMouseMode(window, true);
        return true;
    }

    void BeginFrame() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }
    void DrawLine(const Vector2 start, const Vector2 end) {
        SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
    }

    static float timer = 0;
    static float timerHelper = 0;
    static int fps = 0;

    constexpr static float FOV = 90;
    constexpr static float halfFov = FOV * 0.5f;
    constexpr static double fovInRadians = FOV * M_PI / 180.0f;

    static Vector2 screenCentre = {SCREEN_WIDTH * .5f, SCREEN_HEIGHT * .5f};

    constexpr float nearPlane = 0.01f;
    constexpr float wallBottom = 0.0f;
    constexpr float wallTop = 64.0f;
    constexpr float eyeHeight = 32.0f;
    static double projectionScale = SCREEN_WIDTH * .5f / std::tan(fovInRadians * .5f);

    void UpdateFrame(const Player &player, const std::vector<Wall> &walls) {
        timer = GameTime::time;

        const Vector2 pos = player.GetPosition();
        const float angle = player.GetAngle();
        const float angleInRadians = angle * M_PI / 180.0f;

        const float leftAngle = angle + halfFov;
        const float rightAngle = angle - halfFov;

        const float leftAngleInRadians = leftAngle * M_PI / 180.0f;
        const float rightAngleInRadians = rightAngle * M_PI / 180.0f;

        const float sin = std::sin(angleInRadians);
        const float cos = std::cos(angleInRadians);

        const Vector2 forward = {sin, cos};
        const Vector2 right = {cos, -sin};

        auto Lerp = [](const float a, const float b, const float t)->float { return a + (b - a) * t; };

        for (const Wall& wall : walls) {
            const Vector2 wallVector = wall.start - wall.end;
            const Vector2 relStart = wall.start - player.GetPosition();
            const Vector2 relEnd = wall.end - player.GetPosition();

            float startCamX = relStart.Dot(right);
            float startCamZ = relStart.Dot(forward);
            float endCamX = relEnd.Dot(right);
            float endCamZ = relEnd.Dot(forward);

            if (startCamZ <= nearPlane && endCamZ <= nearPlane) continue;
            if (startCamZ <= nearPlane) {
                const float t = (nearPlane - startCamZ) / (endCamZ - startCamZ);
                startCamX = Lerp(startCamX, endCamX, t);
                startCamZ = nearPlane;
            }

            if (endCamZ <= nearPlane) {
                const float t = (nearPlane - endCamZ) / (startCamZ - endCamZ);
                endCamX = Lerp(endCamX, startCamX, t);
                endCamZ = nearPlane;
            }

            double startScreenX = screenCentre.x + (startCamX / startCamZ) * projectionScale;
            double endScreenX = screenCentre.x + (endCamX / endCamZ) * projectionScale;

            if ((startScreenX < 0.0f && endScreenX < 0.0f) || (startScreenX >= SCREEN_WIDTH && endScreenX >= SCREEN_WIDTH)) continue;

            float startYTop = screenCentre.y - ((wallTop - eyeHeight) / startCamZ) * projectionScale;
            float startYBot = screenCentre.y - ((wallBottom - eyeHeight) / startCamZ) * projectionScale;
            float endYTop = screenCentre.y - ((wallTop - eyeHeight) / endCamZ) * projectionScale;
            float endYBot = screenCentre.y - ((wallBottom - eyeHeight) / endCamZ) * projectionScale;

            if (startScreenX > endScreenX) {
                std::swap(startScreenX, endScreenX);
                std::swap(startYTop, endYTop);
                std::swap(startYBot, endYBot);
            }

            int xStart = std::max(0, static_cast<int>(std::ceil(startScreenX)));
            int xEnd = std::min(SCREEN_WIDTH - 1, static_cast<int>(std::floor(endScreenX)));

            if (xStart > xEnd) continue;

            SDL_SetRenderDrawColor(renderer, 100, 80, 90, 255);
            for (int x = xStart; x <= xEnd; x++) {
                float t = endScreenX == startScreenX ? 0.0f : (x - startScreenX) / (endScreenX - startScreenX);
                float yTop = startYTop + (endYTop - startYTop) * t;
                float yBot = startYBot + (endYBot - startYBot) * t;
                DrawLine({static_cast<float>(x), yTop}, {static_cast<float>(x), yBot});
            }
        }

        // ---------- PLAYER DEBUG PASS ----------
        if (DEBUG) {
            for (const auto&[start, end] : walls) {
                const Vector2 relStart = start - player.GetPosition();
                const Vector2 relEnd = end - player.GetPosition();
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                const Vector2 screenStart = {
                    screenCentre.x + relStart.x,
                    screenCentre.y - relStart.y
                };

                const Vector2 screenEnd = {
                    screenCentre.x + relEnd.x,
                    screenCentre.y - relEnd.y
                };

                DrawLine(screenStart, screenEnd);
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            const SDL_FRect pRect = {
                screenCentre.x - player.GetSize() * 0.5f,
                screenCentre.y - player.GetSize() * 0.5f,
                player.GetSize(),
                player.GetSize()
            };
            constexpr float debugLength = 1000.0f;

            const Vector2 leftDir = {
                std::sin(leftAngleInRadians),
                std::cos(leftAngleInRadians)
            };

            const Vector2 rightDir = {
                std::sin(rightAngleInRadians),
                std::cos(rightAngleInRadians)
            };
            const Vector2 leftEnd = {
                screenCentre.x + leftDir.x * debugLength,
                screenCentre.y - leftDir.y * debugLength
            };

            const Vector2 rightEnd = {
                screenCentre.x + rightDir.x * debugLength,
                screenCentre.y - rightDir.y * debugLength
            };
            DrawLine(screenCentre, leftEnd);
            DrawLine(screenCentre, rightEnd);

            SDL_RenderFillRect(renderer, &pRect);
        }

        if (timer > timerHelper + FPS_COOLDOWN_SECONDS) {
            fps = static_cast<int>(GameTime::GetFPS());
            timerHelper = timer;
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_SetRenderScale(renderer, 3.5f, 3.5f);
        SDL_RenderDebugText(renderer, 0, 0, std::to_string(fps).c_str());
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);

        SDL_RenderPresent(renderer);
    }

}