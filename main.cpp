#include <iostream>
#include <iterator>
#include <vector>

#include "Headers/Engine/InputManager.h"
#include "Headers/Engine/GameTime.h"

#include "Headers/Objects/Player.h"
#include "Headers/Objects/Wall.h"

#include "Headers/Renderer/MapEditor.h"
#include "Headers/Renderer/Renderer.h"
#include "Headers/Renderer/BSP.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

int main() {
    Renderer::Initialize();

    auto MakeWall = [](Vector2 a, Vector2 b, Vector3 c, int front, int back = -1) {
        Wall w(a, b, c);
        w.frontSector = front;
        w.backSector = back;
        return w;
    };

    Sector sectors[] = {
        {  0.0f, 48.0f }, // 0 top-left : normal
        {  8.0f, 48.0f }, // 1 top-right : raised floor, same ceiling
        {  0.0f, 32.0f }, // 2 bottom-left : same floor, lower ceiling
        { -8.0f, 40.0f }, // 3 bottom-right : sunken pool
    };

    const std::vector<Wall> walls = {
        // ---------- OUTER BOUNDARY ----------
        MakeWall({-220,  160}, {   0,  160}, {110, 110, 115}, 0),
        MakeWall({   0,  160}, { 220,  160}, {120, 120, 125}, 1),

        MakeWall({ 220,  160}, { 220,    0}, {130, 130, 135}, 1),
        MakeWall({ 220,    0}, { 220, -160}, {140, 140, 145}, 3),

        MakeWall({ 220, -160}, {   0, -160}, {135, 135, 140}, 3),
        MakeWall({   0, -160}, {-220, -160}, {125, 125, 130}, 2),

        MakeWall({-220, -160}, {-220,    0}, {120, 120, 125}, 2),
        MakeWall({-220,    0}, {-220,  160}, {110, 110, 115}, 0),

        // ---------- INTERNAL PORTAL WALLS ----------
        // vertical top divider: sector 1 on the right, sector 0 on the left
        MakeWall({   0,  160}, {   0,    0}, {255, 120, 120}, 1, 0),

        // vertical bottom divider: sector 3 on the right, sector 2 on the left
        MakeWall({   0,    0}, {   0, -160}, {120, 255, 120}, 3, 2),

        // horizontal left divider: sector 0 above, sector 2 below
        MakeWall({-220,    0}, {   0,    0}, {120, 120, 255}, 0, 2),

        // horizontal right divider: sector 1 above, sector 3 below
        MakeWall({   0,    0}, { 220,    0}, {255, 255, 120}, 1, 3),
    };

    const std::vector<SectorPolygon> sectorPolygons = {
        {
            {
                {-220,    0},
                {   0,    0},
                {   0,  160},
                {-220,  160}
            },
            0
        },
        {
            {
                {   0,    0},
                { 220,    0},
                { 220,  160},
                {   0,  160}
            },
            1
        },
        {
            {
                {-220, -160},
                {   0, -160},
                {   0,    0},
                {-220,    0}
            },
            2
        },
        {
            {
                {   0, -160},
                { 220, -160},
                { 220,    0},
                {   0,    0}
            },
            3
        }
    };

    for (const Wall& w : walls) MapEditor::CreateWallDirectly(w);
    for (const Sector& s : sectors) MapEditor::CreateSectorDirectly(s);

    Player player({-110, 80}, 95.0f, 10.0f);

    const auto tree = BuildBSP(MapEditor::walls);
    BuildSubSectors(tree.get(), sectorPolygons);

    bool running = true;
    while (running) {
        Renderer::BeginFrame();
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update(MapEditor::walls, MapEditor::sectors);

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

        player.SetCurrentSector(FindSector(tree.get(), player.GetPosition()));

       // std::vector<Wall> orderedWalls;
       // CollectBSPWalls(tree.get(), player.GetPosition(), orderedWalls);

        Renderer::UpdateFrame(player, MapEditor::walls, MapEditor::sectors);
    }

    return 0;
}