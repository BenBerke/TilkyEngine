//
// Created by berke on 4/8/2026.
//

#ifndef TILKYENGINE_RENDERER_H
#define TILKYENGINE_RENDERER_H

#include <vector>

#include "../Objects/Wall.h"
#include "../Objects/Player.h"

namespace Renderer {
    bool Initialize();
    void BeginFrame();
    void UpdateFrame(const Player &player, const std::vector<Wall> &walls);
}

#endif //TILKYENGINE_RENDERER_H