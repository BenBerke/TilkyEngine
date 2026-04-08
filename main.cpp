#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>

#include "Headers/Engine/InputManager.h"
#include "Headers/Engine/GameTime.h"

#include "Headers/Objects/Player.h"
#include "Headers/Objects/Wall.h"

#include "Headers/Renderer/MapEditor.h"
#include "Headers/Renderer/Renderer.h"


#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

int main() {
    Renderer::Initialize();

    // ----- Room 1 (left room) -----
    // corners: (0,0), (250,0), (250,250), (0,250)

    Wall w1 = {{0, 0}, {250, 0}};
    Wall w2 = {{0, 250}, {0, 0}};
    Wall w3 = {{250, 250}, {0, 250}};
    Wall w4 = {{250, 0}, {250, 250}};

    // ----- Room 2 (right room) -----
    // corners: (250,0), (500,0), (500,250), (250,250)

    Wall w5 = {{250, 0}, {500, 0}};
    Wall w6 = {{500, 0}, {500, 250}};
    Wall w7 = {{500, 250}, {250, 250}};

    MapEditor::CreateWallDirectly(w1);
    MapEditor::CreateWallDirectly(w2);
    MapEditor::CreateWallDirectly(w3);
    MapEditor::CreateWallDirectly(w4);
    MapEditor::CreateWallDirectly(w5);
    MapEditor::CreateWallDirectly(w6);
    MapEditor::CreateWallDirectly(w7);

    Player player({125, 125}, 95.0f, 10.0f);

    bool running = true;
    while (running) {
        Renderer::BeginFrame();
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update();

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

        std::cout << InputManager::GetMouseDelta().x << std::endl;

        Renderer::UpdateFrame(player, MapEditor::walls);
    }

    return 0;
}
