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

    if (walls.size() == 1) {
        node->isLeaf = true;
        return node;
    }

    std::vector<Wall> leftWalls, rightWalls;

    for (size_t i = 1; i < walls.size(); i++) {
        const Wall& current = walls[i];
        float startSide = PointSide(node->partition, current.start);
        float endSide = PointSide(node->partition, current.end);

        if (startSide >= 0 && endSide >= 0) {
            leftWalls.push_back(current);
        }
        else if (startSide <= 0 && endSide <= 0) {
            rightWalls.push_back(current);
        }
        else {
            leftWalls.push_back(current);
        }
    }

    node->left = BuildTree(leftWalls);
    node->right = BuildTree(rightWalls);

    return node;
}

void TraverseTree(const BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls) {
    if (!node) return;

    float side = PointSide(node->partition, playerPos);
    if (side >= 0) {
        TraverseTree(node->right.get(), playerPos, outWalls);
        outWalls.push_back(node->partition);
        TraverseTree(node->left.get(), playerPos, outWalls);
    }
    else {
        TraverseTree(node->left.get(), playerPos, outWalls);
        outWalls.push_back(node->partition);
        TraverseTree(node->right.get(), playerPos, outWalls);
    }
}