//
// Created by berke on 4/8/2026.
//

#ifndef TILKYENGINE_BSP_H
#define TILKYENGINE_BSP_H

#include <memory>
#include <vector>

#include "../Objects/Wall.h"
#include "../Math/Vector.h"

struct BSPNode {
    std::vector<Wall> walls;

    Wall splitter;

    std::vector<Wall> coplanar;

    std::unique_ptr<BSPNode> front;
    std::unique_ptr<BSPNode> back;

    explicit BSPNode(const Wall& splitter) : splitter(splitter) {}

    int frontCount = 0, backCount = 0, spanningCount = 0;

};

std::unique_ptr<BSPNode> BuildBSP(const std::vector<Wall>& walls);
void CollectBSPWalls(BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls);

#endif //TILKYENGINE_BSP_H