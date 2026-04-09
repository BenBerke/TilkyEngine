//
// Created by berke on 4/8/2026.
//

#include <limits>
#include <cmath>
#include <utility>

#include "../../Headers/Renderer/BSP.h"

constexpr float EPSILON = 1e-4f;

enum class SegmentSide {
    Front,
    Back,
    Spanning,
    Coplanar,
};

static float PointSide(const Wall& splitter, const Vector2& point) {
    return splitter.dir.Cross(point - splitter.start);
}

static SegmentSide ClassifyWall(const Wall& splitter, const Wall& wall) {
    const float d1 = PointSide(splitter, wall.start);
    const float d2 = PointSide(splitter, wall.end);

    const bool sOn = std::fabs(d1) <= EPSILON;
    const bool eOn = std::fabs(d2) <= EPSILON;

    if (sOn && eOn) return SegmentSide::Coplanar;
    if (d1 >= -EPSILON && d2 >= -EPSILON) return SegmentSide::Front;
    if (d1 <= EPSILON && d2 <= EPSILON) return SegmentSide::Back;
    return SegmentSide::Spanning;
}

static std::pair<Wall, Wall> SplitWall(const Wall& splitter, const Wall& wall) {
    const float d1 = PointSide(splitter, wall.start);
    const float d2 = PointSide(splitter, wall.end);

    const float t = d1 / (d1 - d2);
    const Vector2 intersection = wall.start + wall.dir * t;

    if (d1 > 0.0f) {
        Wall frontPart{wall.start, intersection, wall.color, wall.frontSector, wall.backSector};
        Wall backPart{intersection, wall.end, wall.color, wall.frontSector, wall.backSector};
        return {frontPart, backPart};
    } else {
        Wall backPart{wall.start, intersection, wall.color, wall.frontSector, wall.backSector};
        Wall frontPart{intersection, wall.end, wall.color, wall.frontSector, wall.backSector};
        return {frontPart, backPart};
    }
}

static int ScoreSplitter(const std::vector<Wall>& walls, size_t splitterIndex) {
    int frontCount = 0;
    int backCount = 0;
    int spanningCount = 0;

    const Wall& splitter = walls[splitterIndex];

    for (size_t i = 0; i < walls.size(); i++) {
        if (i == splitterIndex) continue;

        switch (ClassifyWall(splitter, walls[i])) {
            case SegmentSide::Front:
                frontCount++;
                break;
            case SegmentSide::Back:
                backCount++;
                break;
            case SegmentSide::Spanning:
                spanningCount++;
                break;
            case SegmentSide::Coplanar:
                break;
        }
    }

    return std::abs(frontCount - backCount) + spanningCount * 8;
}

std::unique_ptr<BSPNode> BuildBSP(const std::vector<Wall>& walls) {
    if (walls.empty()) return nullptr;

    int currentMin = std::numeric_limits<int>::max();
    size_t minIndex = 0;

    for (size_t i = 0; i < walls.size(); ++i) {
        const int score = ScoreSplitter(walls, i);
        if (score < currentMin) {
            currentMin = score;
            minIndex = i;
        }
    }

    auto node = std::make_unique<BSPNode>(walls[minIndex]);
    node->coplanar.push_back(node->splitter);

    std::vector<Wall> frontWalls;
    std::vector<Wall> backWalls;

    for (size_t i = 0; i < walls.size(); i++) {
        if (i == minIndex) continue;

        const Wall& wall = walls[i];
        const SegmentSide side = ClassifyWall(node->splitter, wall);

        switch (side) {
            case SegmentSide::Coplanar:
                node->coplanar.push_back(wall);
                break;

            case SegmentSide::Front:
                frontWalls.push_back(wall);
                break;

            case SegmentSide::Back:
                backWalls.push_back(wall);
                break;

            case SegmentSide::Spanning: {
                auto [frontPart, backPart] = SplitWall(node->splitter, wall);
                frontWalls.push_back(frontPart);
                backWalls.push_back(backPart);
                break;
            }
        }
    }

    node->front = BuildBSP(frontWalls);
    node->back = BuildBSP(backWalls);

    return node;
}

void CollectBSPWalls(BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls) {
    if (!node) return;

    const float side = PointSide(node->splitter, playerPos);

    if (side >= 0.0f) {
        CollectBSPWalls(node->back.get(), playerPos, outWalls);
        outWalls.insert(outWalls.end(), node->coplanar.begin(), node->coplanar.end());
        CollectBSPWalls(node->front.get(), playerPos, outWalls);
    } else {
        CollectBSPWalls(node->front.get(), playerPos, outWalls);
        outWalls.insert(outWalls.end(), node->coplanar.begin(), node->coplanar.end());
        CollectBSPWalls(node->back.get(), playerPos, outWalls);
    }
}