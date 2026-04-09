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

static std::vector<Vector2> RemoveNearDuplicateVertices(const std::vector<Vector2>& poly) {
    std::vector<Vector2> out;
    out.reserve(poly.size());
    for (const Vector2& v : poly) {
        if (out.empty() || (v - out.back()).Dot(v - out.back()) > EPSILON * EPSILON) out.push_back(v);
    }

    if (out.size() >= 2) {
        Vector2 diff = out.front() - out.back();
        if (diff.Dot(diff) < EPSILON * EPSILON) out.pop_back();
    }

    return out;
}

static std::pair<std::vector<Vector2>, std::vector<Vector2>> SplitPolygonByLine(const std::vector<Vector2>& poly, const Wall& splitter) {
    std::vector<Vector2> frontPoly, backPoly;
    if (poly.size() < 3) return {frontPoly, backPoly};

    for (size_t i = 0; i < poly.size(); i++) {
        const Vector2 current = poly[i];
        const Vector2 next = poly[(i + 1) % poly.size()];
        const float currentSide = PointSide(splitter, current);
        const float nextSide = PointSide(splitter, next);
        const bool currentFront = currentSide >= -EPSILON;
        const bool currentBack = currentSide <= EPSILON;
        const bool crosses = (currentSide > EPSILON && nextSide < -EPSILON || currentSide < -EPSILON && nextSide > EPSILON);

        if (currentFront) frontPoly.push_back(current);
        if (currentBack) backPoly.push_back(current);
        if (crosses) {
            const float t = currentSide / (currentSide - nextSide);
            const Vector2 intersection = current + (next - current) * t;
            frontPoly.push_back(intersection);
            backPoly.push_back(intersection);
        }
    }

    frontPoly = RemoveNearDuplicateVertices(frontPoly);
    backPoly = RemoveNearDuplicateVertices(backPoly);

    return {frontPoly, backPoly};
}

static void AddPolygonToLeaves(BSPNode* node, const std::vector<Vector2>& polygon, int sectorId) {
    if (!node || polygon.size() < 3) return;

    if (node->isLeaf) {
        if (!node->subSector) {
            node->subSector = std::make_unique<SubSector>();
            node->subSector->sectorId = sectorId;
            node->subSector->polygon = polygon;
        }
        return;
    }
    auto [frontPoly, backPoly] = SplitPolygonByLine(polygon, node->splitter);
    if (!frontPoly.empty()) AddPolygonToLeaves(node->front.get(), frontPoly, sectorId);
    if (!backPoly.empty()) AddPolygonToLeaves(node->back.get(), backPoly, sectorId);
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

    if (frontWalls.empty()) {
        node->front = std::make_unique<BSPNode>(node->splitter);
        node->front->isLeaf = true;
    }
    else node->front = BuildBSP(frontWalls);
    if (backWalls.empty()) {
        node->back = std::make_unique<BSPNode>(node->splitter);
        node->back->isLeaf = true;
    }
    else node->back = BuildBSP(backWalls);

    return node;
}

void BuildSubSectors(BSPNode* root, const std::vector<SectorPolygon>& sectorPolygons) {
    if (!root) return;

    for (const SectorPolygon& polygon : sectorPolygons) AddPolygonToLeaves(root, polygon.vertices, polygon.sectorId);
}

const SubSector* FindSubSector(const BSPNode* node, const Vector2& playerPos) {
    const BSPNode* current = node;

    while (current && !current->isLeaf) {
        const float side = PointSide(current->splitter, playerPos);

        if (side >= 0.0f) current = current->front.get();
        else current = current->back.get();
    }

    if (!current || !current->subSector) return nullptr;
    return current->subSector.get();
}

int FindSector(const BSPNode* node, const Vector2& playerPos) {
    const SubSector* subSector = FindSubSector(node, playerPos);
    if (!subSector) return -1;
    return subSector->sectorId;
}

void CollectBSPWalls(BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls) {
    if (!node || node->isLeaf) return;

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