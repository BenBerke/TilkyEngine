#include "../../Headers/Renderer/BSP.h"

#include <algorithm>

static float PointSide(const Wall& partition, const Vector2& point) {
    Vector2 line = {
        partition.end.x - partition.start.x,
        partition.end.y - partition.start.y
    };

    Vector2 toPoint = {
        point.x - partition.start.x,
        point.y - partition.start.y
    };

    return line.Cross(toPoint);
}

std::unique_ptr<BSPNode> BSPNode::BuildTree(const std::vector<Wall>& walls) {
    if (walls.empty()) return nullptr;

    auto node = std::make_unique<BSPNode>();
    node->partition = walls[0];

    std::vector<Wall> frontWalls, backWalls;

    for (size_t i = 1; i < walls.size(); i++) {
        const Wall& current = walls[i];
        float startSide = PointSide(node->partition, current.start);
        float endSide = PointSide(node->partition, current.end);

        if (startSide >= 0 && endSide >= 0) {
            frontWalls.push_back(current);
        }
        else if (startSide <= 0 && endSide <= 0) {
            backWalls.push_back(current);
        }
        else {
            frontWalls.push_back(current);
        }
    }

    if (!frontWalls.empty()) node->front = BuildTree(frontWalls);
    else {
        node->front = std::make_unique<BSPNode>();
        node->front->isLeaf = true;
        node->front->sectorId = node->partition.frontSector;
    }
    if (!backWalls.empty()) node->back = BuildTree(backWalls);
    else {
        node->back = std::make_unique<BSPNode>();
        node->back->isLeaf = true;
        node->back->sectorId = node->partition.backSector;
    }

    return node;
}

void TraverseTree(const BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls) {
    if (!node) return;

    float side = PointSide(node->partition, playerPos);
    if (side >= 0) {
        TraverseTree(node->back.get(), playerPos, outWalls);
        outWalls.push_back(node->partition);
        TraverseTree(node->front.get(), playerPos, outWalls);
    }
    else {
        TraverseTree(node->front.get(), playerPos, outWalls);
        outWalls.push_back(node->partition);
        TraverseTree(node->back.get(), playerPos, outWalls);
    }
}

int FindPlayerSector(const BSPNode* node, const Vector2& playerPos) {
    if (!node) return -1;
    if (node->isLeaf) return node->sectorId;

    float side = PointSide(node->partition, playerPos);

    if (side >= 0) return FindPlayerSector(node->front.get(), playerPos);
    return FindPlayerSector(node->back.get(), playerPos);
}