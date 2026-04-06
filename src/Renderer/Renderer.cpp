//
// Created by berke on 4/6/2026.
//

#include <algorithm>
#include <math.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>

#include "../../Headers/Renderer/Renderer.h"
#include "../../Headers/Math/Vector.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

namespace {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
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
        return true;
    }

    void BeginFrame() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }

    void DrawLine(const Vector2 start, const Vector2 end) {
        SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
    }

    void UpdateFrame(const Player& player, const std::vector<Wall>& walls) {
    const float centreX = SCREEN_WIDTH * 0.5f;
    const float centreY = SCREEN_HEIGHT * 0.5f;

    const float focalLength = 400.0f;
    const float nearPlane = 1.0f;
    const float wallHeight = 100.0f;

    const float angle = player.GetAngle();

    Vector2 forward = { std::sin(angle), std::cos(angle) };
    Vector2 right   = { std::cos(angle), -std::sin(angle) };

    Vector2 screenCentre = { centreX, centreY };

    // ---------- 3D WALL PASS ----------
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (const auto& [start, end] : walls) {
        Vector2 relStart = start - player.GetPosition();
        Vector2 relEnd   = end   - player.GetPosition();

        Vector2 camStart = {
            relStart.x * right.x + relStart.y * right.y,
            relStart.x * forward.x + relStart.y * forward.y
        };

        Vector2 camEnd = {
            relEnd.x * right.x + relEnd.y * right.y,
            relEnd.x * forward.x + relEnd.y * forward.y
        };

        float x1 = camStart.x;
        float z1 = camStart.y;
        float x2 = camEnd.x;
        float z2 = camEnd.y;

        if (z1 <= nearPlane && z2 <= nearPlane) continue;

        if (z1 <= nearPlane) {
            float t = (nearPlane - z1) / (z2 - z1);
            x1 = x1 + (x2 - x1) * t;
            z1 = nearPlane;
        }

        if (z2 <= nearPlane) {
            float t = (nearPlane - z2) / (z1 - z2);
            x2 = x2 + (x1 - x2) * t;
            z2 = nearPlane;
        }

        float screenX1 = centreX + (x1 * focalLength) / z1;
        float screenX2 = centreX + (x2 * focalLength) / z2;

        float projectedHeight1 = (wallHeight * focalLength) / z1;
        float projectedHeight2 = (wallHeight * focalLength) / z2;

        float topY1 = centreY - projectedHeight1 * 0.5f;
        float botY1 = centreY + projectedHeight1 * 0.5f;

        float topY2 = centreY - projectedHeight2 * 0.5f;
        float botY2 = centreY + projectedHeight2 * 0.5f;

        if (screenX1 > screenX2) {
            std::swap(screenX1, screenX2);
            std::swap(topY1, topY2);
            std::swap(botY1, botY2);
        }

        int xStart = std::max(0, static_cast<int>(std::ceil(screenX1)));
        int xEnd   = std::min(SCREEN_WIDTH - 1, static_cast<int>(std::floor(screenX2)));

        if (xEnd < xStart) continue;

        for (int x = xStart; x <= xEnd; x++) {
            float t = (x - screenX1) / (screenX2 - screenX1);

            float topY = topY1 + (topY2 - topY1) * t;
            float botY = botY1 + (botY2 - botY1) * t;

            SDL_RenderLine(renderer, static_cast<float>(x), topY,
                                     static_cast<float>(x), botY);
        }
    }

    // ---------- TOP-DOWN DEBUG PASS ----------
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    for (const auto& [start, end] : walls) {
        Vector2 relativeStart = start - player.GetPosition();
        Vector2 relativeEnd   = end   - player.GetPosition();

        Vector2 screenStart = {
            screenCentre.x + relativeStart.x,
            screenCentre.y - relativeStart.y
        };

        Vector2 screenEnd = {
            screenCentre.x + relativeEnd.x,
            screenCentre.y - relativeEnd.y
        };

        DrawLine(screenStart, screenEnd);
    }

    // ---------- PLAYER DEBUG PASS ----------
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    const SDL_FRect pRect = {
        screenCentre.x - player.GetSize() * 0.5f,
        screenCentre.y - player.GetSize() * 0.5f,
        player.GetSize(),
        player.GetSize()
    };
    SDL_RenderFillRect(renderer, &pRect);

    // Facing direction line
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    Vector2 facingEnd = {
        screenCentre.x + forward.x * 30.0f,
        screenCentre.y - forward.y * 30.0f
    };

    DrawLine(screenCentre, facingEnd);

    SDL_RenderPresent(renderer);
}
}
