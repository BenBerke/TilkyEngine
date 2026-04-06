//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_RENDERER_H
#define TILKYENGINE_RENDERER_H

#include <vector>

#include "../Objects/Wall.h"
#include "../Objects/Player.h"

namespace Renderer {
    bool Initialize();
    void BeginFrame();
    void UpdateFrame(const Player &player, const std::vector<Wall>& walls);
    void DrawLine(float x1, float y1, float x2, float y2);
}

#endif //TILKYENGINE_RENDERER_H