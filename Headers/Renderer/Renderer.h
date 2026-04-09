//
// Created by berke on 4/8/2026.
//

#ifndef TILKYENGINE_RENDERER_H
#define TILKYENGINE_RENDERER_H

#include <vector>

#include "../Objects/Wall.h"
#include "../Objects/Player.h"
#include "../Objects/Sector.h"

namespace Renderer {
    bool Initialize();
    void BeginFrame();
    void UpdateFrame(const Player &player, const std::vector<Wall> &walls, const std::vector<Sector> &sectors);
}

#endif //TILKYENGINE_RENDERER_H