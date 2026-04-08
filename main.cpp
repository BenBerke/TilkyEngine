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
        // ---------- OUTER BOUNDARY ----------
        {{-400, -300}, { 400, -300}, {120, 120, 140}},
        {{ 400, -300}, { 400,  300}, {120, 140, 180}},
        {{ 400,  300}, {-400,  300}, {100,  80, 140}},
        {{-400,  300}, {-400, -300}, {140, 100, 100}},

        // ---------- CENTRAL OCTAGON ----------
        {{ -80, -140}, {  80, -140}, {255, 100, 100}},
        {{  80, -140}, { 140,  -80}, {255, 140, 100}},
        {{ 140,  -80}, { 140,   80}, {255, 180, 100}},
        {{ 140,   80}, {  80,  140}, {255, 220, 100}},
        {{  80,  140}, { -80,  140}, {200, 255, 100}},
        {{ -80,  140}, {-140,   80}, {120, 255, 120}},
        {{-140,   80}, {-140,  -80}, {100, 220, 255}},
        {{-140,  -80}, { -80, -140}, {160, 120, 255}},

        // ---------- LEFT DIAMOND ----------
        {{-280,    0}, {-220,  -80}, {255,   0, 120}},
        {{-220,  -80}, {-160,    0}, {255,  80, 160}},
        {{-160,    0}, {-220,   80}, {255, 120, 200}},
        {{-220,   80}, {-280,    0}, {255,  80, 120}},

        // ---------- RIGHT TRIANGULAR WING ----------
        {{ 220,  -90}, { 320,    0}, { 80, 255, 180}},
        {{ 320,    0}, { 220,   90}, { 80, 200, 255}},
        {{ 220,   90}, { 220,  -90}, { 80, 140, 255}},

        // ---------- SMALL INNER BOX ----------
        {{ -40,  -40}, {  40,  -40}, {255, 255, 255}},
        {{  40,  -40}, {  40,   40}, {220, 220, 220}},
        {{  40,   40}, { -40,   40}, {180, 180, 180}},
        {{ -40,   40}, { -40,  -40}, {140, 140, 140}},

        // ---------- TOP-LEFT SLANTED SHAPE ----------
        {{-340, -220}, {-250, -260}, {255, 180,  60}},
        {{-250, -260}, {-180, -180}, {255, 220, 100}},
        {{-180, -180}, {-300, -140}, {255, 140,  60}},
        {{-300, -140}, {-340, -220}, {220, 100,  40}},

        // ---------- BOTTOM-RIGHT TRAPEZOID ----------
        {{ 180,  170}, { 300,  170}, {100, 255, 100}},
        {{ 300,  170}, { 340,  250}, {100, 255, 160}},
        {{ 340,  250}, { 140,  250}, {100, 220, 255}},
        {{ 140,  250}, { 180,  170}, {120, 180, 255}},
    };

    for (Wall w : walls) MapEditor::CreateWallDirectly(w);

    Player player({125, 125}, 95.0f, 10.0f);

    const auto tree = BuildBSP(MapEditor::walls);

    bool running = true;
    while (running) {
        Renderer::BeginFrame();
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update(MapEditor::walls);

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;

        std::vector<Wall> orderedWalls;
        CollectBSPWalls(tree.get(), player.GetPosition(), orderedWalls);

        Renderer::UpdateFrame(player, orderedWalls);
    }

    return 0;
}
