//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_BSP_H
#define TILKYENGINE_BSP_H

#include <memory>
#include <vector>

#include "../Objects/Wall.h"
#include "../Math/Vector.h"

struct BSPNode {
    Wall partition;
    std::unique_ptr<BSPNode> front, back;

    static std::unique_ptr<BSPNode> BuildTree(const std::vector<Wall>& walls);

    bool isLeaf = false;
    int sectorId = -1;
};

void TraverseTree(const BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls);
int FindPlayerSector(const BSPNode* node, const Vector2& playerPos);


#endif //TILKYENGINE_BSP_H