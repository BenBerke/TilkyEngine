#include <iterator>

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

    Wall walls[] = {
        {{0, 0}, {250, 0}, {100, 100, 100}},
        {{0, 250}, {0, 0}, {150, 0, 90}},
        {{250, 250}, {0, 250}, {100, 50, 60}},
        {{250, 0}, {250, 250}, {10, 90, 20}},
        {{250, 0}, {500, 0}, {200, 160, 76}},
        {{500, 0}, {500, 250}, {255, 0, 86}},
        {{500, 250}, {250, 250}, {255, 255, 50}},
    };

    for (Wall w : walls) MapEditor::CreateWallDirectly(w);

    Player player({125, 125}, 95.0f, 10.0f);

    const auto tree = BuildBSP(MapEditor::walls);

    bool running = true;
    while (running) {
        Renderer::BeginFrame();
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update();

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

        std::vector<Wall> orderedWalls;
        CollectBSPWalls(tree.get(), player.GetPosition(), orderedWalls);

        Renderer::UpdateFrame(player, orderedWalls);
    }

    return 0;
}
