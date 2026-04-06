#include <iostream>
#include <SDL3/SDL.h>

#include "Headers/Engine/InputManager.h"
#include "Headers/Objects/Player.h"
#include "Headers/Engine/GameTime.h"
#include "Headers/Renderer/MapEditor.h"
#include "Headers/Renderer/BSP.h"
#include "Headers/Renderer/Renderer.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

int main() {
    Renderer::Initialize();

    std::unique_ptr<BSPNode> bspTree = nullptr;
    Player player((Vector2){0, 0});

    bool editing = true;

    Vector2 firstClick;
    Vector2 secondClick;

    bool running = true;
    while (running) {
        Renderer::BeginFrame();
        InputManager::BeginFrame();
        GameTime::Update();
        player.Update();

        if (InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE)) running = false;
        if (InputManager::GetKeyDown(SDL_SCANCODE_E)) editing = !editing;

        if (InputManager::GetKeyDown(SDL_SCANCODE_B)) {
            if (!MapEditor::walls.empty()) bspTree = BSPNode::BuildTree(MapEditor::walls);
            std::cout << "Tree built" << std::endl;
        }

        if (editing) {
            if (InputManager::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
                firstClick = InputManager::GetMousePosition();
                firstClick.x = firstClick.x + player.GetPosition().x - SCREEN_WIDTH / 2.0f;
                firstClick.y = player.GetPosition().y + SCREEN_HEIGHT / 2.0f - firstClick.y;
            }

            if (InputManager::GetMouseButtonDown(SDL_BUTTON_RIGHT)) {
                secondClick = InputManager::GetMousePosition();
                secondClick.x = secondClick.x + player.GetPosition().x - SCREEN_WIDTH / 2.0f;
                secondClick.y = player.GetPosition().y + SCREEN_HEIGHT / 2.0f - secondClick.y;
            }

            if (InputManager::GetKeyDown(SDL_SCANCODE_R)) {
                MapEditor::CreateWall(firstClick, secondClick);
            }
        }

        std::vector<Wall> renderOrder;
        TraverseTree(bspTree.get(), player.GetPosition(), renderOrder);

        if (bspTree) Renderer::UpdateFrame(player, renderOrder);
        else Renderer::UpdateFrame(player, MapEditor::walls);
    }

    return 0;
}
