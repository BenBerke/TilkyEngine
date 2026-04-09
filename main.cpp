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

auto MakeWall = [](Vector2 a, Vector2 b, Vector3 c, int front, int back = -1) {
    Wall w(a, b, c);
    w.frontSector = front;
    w.backSector = back;
    return w;
};
    Sector sectors[] = {
        {  0.0f,  48.0f }, // 0 Main Floor (Baseline)
        {  8.0f,  56.0f }, // 1 Low Tier (Step up)
        { 16.0f,  64.0f }, // 2 Mid Tier (Step up again)
        { 24.0f,  72.0f }, // 3 High Tier (Waist high to a size 12 player)
        { -8.0f,  40.0f }, // 4 Shallow Sunken Pool (Step down)
        { 12.0f,  48.0f }, // 5 The Dais (Small platform)
    };


    const std::vector walls = {
    // ---------- OUTER BOUNDARY (Sector 0) ----------
    MakeWall({-200, -200}, { 200, -200}, {100, 100, 105}, 0),
    MakeWall({ 200, -200}, { 200,  200}, {110, 110, 115}, 0),
    MakeWall({ 200,  200}, {-200,  200}, {120, 120, 125}, 0),
    MakeWall({-200,  200}, {-200, -200}, {110, 110, 115}, 0),

    // ---------- SHALLOW POOL (Sector 4: Step Down -8) ----------
    MakeWall({-180, -180}, { -80, -180}, { 50,  80, 200}, 0, 4),
    MakeWall({ -80, -180}, { -80,  -80}, { 50,  80, 200}, 0, 4),
    MakeWall({ -80,  -80}, {-180,  -80}, { 50,  80, 200}, 0, 4),
    MakeWall({-180,  -80}, {-180, -180}, { 50,  80, 200}, 0, 4),

    // ---------- LOW TIER (Sector 1: Step Up +8) ----------
    MakeWall({  50, -150}, { 150, -150}, {150, 150, 150}, 0, 1),
    MakeWall({ 150, -150}, { 150,  -50}, {150, 150, 150}, 0, 1),
    MakeWall({ 150,  -50}, {  50,  -50}, {150, 150, 150}, 0, 1),
    MakeWall({  50,  -50}, {  50, -150}, {150, 150, 150}, 0, 1),

    // ---------- MID TIER (Sector 2: Step Up +16) ----------
    // Attached to the Low Tier
    MakeWall({ 150,  -50}, { 150,   50}, {180, 180, 180}, 0, 2),
    MakeWall({ 150,   50}, {  50,   50}, {180, 180, 180}, 0, 2),
    MakeWall({  50,   50}, {  50,  -50}, {180, 180, 180}, 0, 2),
    // (Shared portal edge with Sector 1 would go here if your engine supports sector-to-sector portals)

    // ---------- HIGH TIER (Sector 3: Step Up +24) ----------
    MakeWall({ -50,   80}, {  50,   80}, {210, 210, 210}, 0, 3),
    MakeWall({  50,   80}, {  50,  150}, {210, 210, 210}, 0, 3),
    MakeWall({  50,  150}, { -50,  150}, {210, 210, 210}, 0, 3),
    MakeWall({ -50,  150}, { -50,   80}, {210, 210, 210}, 0, 3),

    // ---------- THE DAIS (Sector 5: Small +12 rise) ----------
    // Right in the center, just high enough for a size 12 player to jump on
    MakeWall({ -20,  -20}, {  20,  -20}, {255, 255, 255}, 0, 5),
    MakeWall({  20,  -20}, {  20,   20}, {255, 255, 255}, 0, 5),
    MakeWall({  20,   20}, { -20,   20}, {255, 255, 255}, 0, 5),
    MakeWall({ -20,   20}, { -20,  -20}, {255, 255, 255}, 0, 5),

    // ---------- SOLID DECORATIVE COLUMN (Solid in Sector 0) ----------
    MakeWall({-150,   50}, {-130,   50}, { 60,  60,  60}, 0),
    MakeWall({-130,   50}, {-130,   70}, { 60,  60,  60}, 0),
    MakeWall({-130,   70}, {-150,   70}, { 60,  60,  60}, 0),
    MakeWall({-150,   70}, {-150,   50}, { 60,  60,  60}, 0),
};

    for (const Wall& w : walls) MapEditor::CreateWallDirectly(w);
    for (const Sector& s : sectors) MapEditor::CreateSectorDirectly(s);

    Player player({0, 50}, 95.0f, 10.0f);

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

        Renderer::UpdateFrame(player, orderedWalls, MapEditor::sectors);
    }

    return 0;
}
