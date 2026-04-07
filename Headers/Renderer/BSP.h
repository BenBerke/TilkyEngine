//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_BSP_H
#define TILKYENGINE_BSP_H

#include <memory>
#include <vector>

#include "../Objects/Wall.h"
#include "../Math/Vector.h"

struct Subsector {
    std::vector<Wall> walls;
    int sectorId = -1;
};

struct BSPNode {
    Wall partition;
    std::vector<Wall> coplanarWalls;

    std::unique_ptr<BSPNode> front;
    std::unique_ptr<BSPNode> back;

    std::unique_ptr<Subsector> subsector;

    bool isLeaf = false;
    int sectorId = -1;

    static std::unique_ptr<BSPNode> BuildTree(const std::vector<Wall>& walls);
};

float PointSide(const Wall& partition, const Vector2& point);
void TraverseTree(const BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls);
int FindPlayerSector(const BSPNode* node, const Vector2& playerPos);
const Subsector* FindPlayerSubsector(const BSPNode* node, const Vector2& playerPos);


#endif //TILKYENGINE_BSP_H