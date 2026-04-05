#include <iostream>
#include <SDL3/SDL.h>

#include "config.h"
#include "Headers/InputManager.h"
#include "Headers/Player.h"
#include "Headers/GameTime.h"


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (SDL_CreateWindowAndRenderer("Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) == false) {
        fprintf(stderr, "Window/Renderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Player player({100, 100}, 150);

    bool running = true;
    while (running) {
        const Uint64 currentTime = SDL_GetTicksNS();

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update();

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

        if (InputManager::GetKey(SDL_SCANCODE_W)) player.SetVelocity({player.GetVelocity().x, -1.0f});
        if (InputManager::GetKey(SDL_SCANCODE_A)) player.SetVelocity({-1.0f, player.GetVelocity().y});
        if (InputManager::GetKey(SDL_SCANCODE_S)) player.SetVelocity({player.GetVelocity().x, 1.0f});
        if (InputManager::GetKey(SDL_SCANCODE_D)) player.SetVelocity({1.0f, player.GetVelocity().y});

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_FRect p = {player.GetPosition().x, player.GetPosition().y, 20, 20};
        SDL_RenderFillRect(renderer, &p);

        SDL_RenderPresent(renderer);
    }

    return 0;
}