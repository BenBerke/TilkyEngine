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

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

#define FPS_COOLDOWN_SECONDS 1.0f

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

    static Vector2 screenCentre = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

    void UpdateFrame(const Player &player, const std::vector<Wall> &walls) {
        timer = GameTime::time;

        const float angle = player.GetAngle();
        Vector2 forward = {std::sin(angle), std::cos(angle)};
        Vector2 right = {std::cos(angle), -std::sin(angle)};

        // ---------- TOP-DOWN DEBUG PASS ----------
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (const Wall &wall: walls) {
            Vector2 relativeStart = wall.start - player.GetPosition();
            Vector2 relativeEnd = wall.end - player.GetPosition();

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
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
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