#include <iostream>
#include <SDL3/SDL.h>

#include "Headers/Engine/InputManager.h"
#include "Headers/Engine/GameTime.h"

#include "Headers/Objects/Player.h"
#include "Headers/Objects/Wall.h"
#include "Headers/Objects/Sector.h"

#include "Headers/Renderer/MapEditor.h"
#include "Headers/Renderer/BSP.h"
#include "Headers/Renderer/Renderer.h"


#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

int main() {
    Renderer::Initialize();

    std::unique_ptr<BSPNode> bspTree = nullptr;
    Player player((Vector2){125, 125});

    Sector s1 = {0, 90};
    Sector s2 = {20, 75};

    MapEditor::CreateSectorDirectly(s1); // sector 0
    MapEditor::CreateSectorDirectly(s2); // sector 1

    // ----- Room 1 (left room) -----
    // corners: (0,0), (250,0), (250,250), (0,250)

    Wall w1 = {{0, 0}, {250, 0}, (Vector3){150, 200, 125},0, -1};
    Wall w2 = {{0, 250}, {0, 0}, (Vector3){150, 100, 125}, 0, -1};
    Wall w3 = {{250, 250}, {0, 250}, (Vector3){25, 200, 60},0, -1};
    Wall w4 = {{250, 0}, {250, 250}, (Vector3){80, 250, 80},0, 1};

    // ----- Room 2 (right room) -----
    // corners: (250,0), (500,0), (500,250), (250,250)

    Wall w5 = {{250, 0}, {500, 0}, (Vector3){200, 200, 125}, 1, -1};
    Wall w6 = {{500, 0}, {500, 250}, (Vector3){100, 100, 125}, 1, -1};
    Wall w7 = {{500, 250}, {250, 250}, (Vector3){100, 200, 55}, 1, -1};

    MapEditor::CreateWallDirectly(w1);
    MapEditor::CreateWallDirectly(w2);
    MapEditor::CreateWallDirectly(w3);
    MapEditor::CreateWallDirectly(w4);
    MapEditor::CreateWallDirectly(w5);
    MapEditor::CreateWallDirectly(w6);
    MapEditor::CreateWallDirectly(w7);

    if (!MapEditor::walls.empty()) bspTree = BSPNode::BuildTree(MapEditor::walls);

    bool running = true;
    while (running) {
        Renderer::BeginFrame();
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update();

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

        std::vector<Wall> renderOrder;
        TraverseTree(bspTree.get(), player.GetPosition(), renderOrder);
        int currentSector = FindPlayerSector(bspTree.get(), player.GetPosition());

        Renderer::UpdateFrame(player, renderOrder, MapEditor::sectors);
    }

    return 0;
}
