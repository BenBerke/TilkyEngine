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

    void UpdateFrame(const Player& player, const std::vector<Wall>& walls, const std::vector<Sector> sectors) {
    const float centreX = SCREEN_WIDTH * 0.5f;
    const float centreY = SCREEN_HEIGHT * 0.5f;

    const float focalLength = 400.0f;
    const float nearPlane = 1.0f;
    constexpr float eyeHeight = 25.0f;

    const float angle = player.GetAngle();

    Vector2 forward = { std::sin(angle), std::cos(angle) };
    Vector2 right   = { std::cos(angle), -std::sin(angle) };

    Vector2 screenCentre = { centreX, centreY };

    // ---------- 3D WALL PASS ----------
    for (const Wall& wall : walls) {
        SDL_SetRenderDrawColor(renderer, wall.color.x, wall.color.y, wall.color.z, 255);
        Vector2 relStart = wall.start - player.GetPosition();
        Vector2 relEnd   = wall.end   - player.GetPosition();

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

        if (wall.frontSector < 0 || wall.frontSector >= static_cast<int>(sectors.size())) continue;
        const Sector& front = sectors[wall.frontSector];

        auto ProjectY = [&](float worldHeight, float depth) {
            return centreY - ((worldHeight - eyeHeight) * focalLength) / depth;
        };

        auto DrawSpan = [&](float ceil1, float floor1, float ceil2, float floor2) {
            float topY1 = ProjectY(ceil1, z1);
            float botY1 = ProjectY(floor1, z1);

            float topY2 = ProjectY(ceil2, z2);
            float botY2 = ProjectY(floor2, z2);

            float drawScreenX1 = screenX1;
            float drawScreenX2 = screenX2;

            if (drawScreenX1 > drawScreenX2) {
                std::swap(drawScreenX1, drawScreenX2);
                std::swap(topY1, topY2);
                std::swap(botY1, botY2);
            }

            if (drawScreenX2 == drawScreenX1) return;

            int xStart = std::max(0, static_cast<int>(std::ceil(drawScreenX1)));
            int xEnd   = std::min(SCREEN_WIDTH - 1, static_cast<int>(std::floor(drawScreenX2)));

            if (xEnd < xStart) return;

            for (int x = xStart; x <= xEnd; x++) {
                float t = (x - drawScreenX1) / (drawScreenX2 - drawScreenX1);

                float topY = topY1 + (topY2 - topY1) * t;
                float botY = botY1 + (botY2 - botY1) * t;

                SDL_RenderLine(renderer, static_cast<float>(x), topY,
                               static_cast<float>(x), botY);
            }
        };

        if (wall.backSector == -1) {
            DrawSpan(front.ceilingHeight, front.floorHeight,
                     front.ceilingHeight, front.floorHeight);
        } else {
            if (wall.backSector < 0 || wall.backSector >= static_cast<int>(sectors.size())) continue;
            const Sector& back = sectors[wall.backSector];

            if (back.floorHeight > front.floorHeight) {
                DrawSpan(back.floorHeight, front.floorHeight,
                         back.floorHeight, front.floorHeight);
            }

            if (back.ceilingHeight < front.ceilingHeight) {
                DrawSpan(front.ceilingHeight, back.ceilingHeight,
                         front.ceilingHeight, back.ceilingHeight);
            }
        }
    }

    // ---------- TOP-DOWN DEBUG PASS ----------
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    for (const Wall& wall : walls) {
        Vector2 relativeStart = wall.start - player.GetPosition();
        Vector2 relativeEnd   = wall.end   - player.GetPosition();

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

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    Vector2 facingEnd = {
        screenCentre.x + forward.x * 30.0f,
        screenCentre.y - forward.y * 30.0f
    };

    DrawLine(screenCentre, facingEnd);

    SDL_RenderPresent(renderer);
}
}
