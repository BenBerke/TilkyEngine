#include "../../Headers/Renderer/BSP.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>

static constexpr float EPSILON = 0.0001f;
static constexpr float MIN_FRAGMENT_LENGTH = 0.5f;
static constexpr float MIN_FRAGMENT_LENGTH_SQ = MIN_FRAGMENT_LENGTH * MIN_FRAGMENT_LENGTH;
static constexpr size_t MAX_SUBSECTOR_WALLS = 2;

enum class WallClassification {
    Coplanar,
    Front,
    Back,
    Spanning
};

float PointSide(const Wall& partition, const Vector2& point) {
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

static Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    };
}

static float WallLengthSq(const Wall& wall) {
    float dx = wall.end.x - wall.start.x;
    float dy = wall.end.y - wall.start.y;
    return dx * dx + dy * dy;
}

static bool IsTinyFragment(const Wall& wall) {
    return WallLengthSq(wall) <= MIN_FRAGMENT_LENGTH_SQ;
}

static WallClassification ClassifyWall(const Wall& partition, const Wall& wall) {
    float startSide = PointSide(partition, wall.start);
    float endSide   = PointSide(partition, wall.end);

    bool startOn = std::fabs(startSide) <= EPSILON;
    bool endOn   = std::fabs(endSide) <= EPSILON;

    bool startFront = startSide > EPSILON;
    bool startBack  = startSide < -EPSILON;
    bool endFront   = endSide > EPSILON;
    bool endBack    = endSide < -EPSILON;

    if (startOn && endOn) {
        return WallClassification::Coplanar;
    }

    if ((startFront || startOn) && (endFront || endOn)) {
        return WallClassification::Front;
    }

    if ((startBack || startOn) && (endBack || endOn)) {
        return WallClassification::Back;
    }

    return WallClassification::Spanning;
}

static void SplitWall(const Wall& partition, const Wall& wall, Wall& frontPiece, Wall& backPiece) {
    float startSide = PointSide(partition, wall.start);
    float endSide   = PointSide(partition, wall.end);

    float t = startSide / (startSide - endSide);
    Vector2 splitPoint = Lerp(wall.start, wall.end, t);

    if (startSide > 0.0f) {
        frontPiece = wall;
        frontPiece.start = wall.start;
        frontPiece.end   = splitPoint;

        backPiece = wall;
        backPiece.start = splitPoint;
        backPiece.end   = wall.end;
    }
    else {
        backPiece = wall;
        backPiece.start = wall.start;
        backPiece.end   = splitPoint;

        frontPiece = wall;
        frontPiece.start = splitPoint;
        frontPiece.end   = wall.end;
    }
}

static size_t ChooseBestSplitter(const std::vector<Wall>& walls) {
    size_t bestIndex = 0;
    float bestScore = std::numeric_limits<float>::infinity();

    for (size_t i = 0; i < walls.size(); i++) {
        const Wall& candidate = walls[i];

        int frontCount = 0;
        int backCount = 0;
        int splitCount = 0;
        int coplanarCount = 0;

        for (size_t j = 0; j < walls.size(); j++) {
            if (i == j) continue;

            switch (ClassifyWall(candidate, walls[j])) {
                case WallClassification::Front:
                    frontCount++;
                    break;
                case WallClassification::Back:
                    backCount++;
                    break;
                case WallClassification::Spanning:
                    splitCount++;
                    break;
                case WallClassification::Coplanar:
                    coplanarCount++;
                    break;
            }
        }

        float balancePenalty = static_cast<float>(std::abs(frontCount - backCount));
        float splitPenalty = static_cast<float>(splitCount) * 8.0f;
        float coplanarPenalty = static_cast<float>(coplanarCount) * 0.25f;

        float score = splitPenalty + balancePenalty + coplanarPenalty;

        if (score < bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }

    return bestIndex;
}

static int ChooseDominantSector(const std::vector<Wall>& walls) {
    std::unordered_map<int, int> counts;

    for (const Wall& wall : walls) {
        if (wall.frontSector >= 0) counts[wall.frontSector]++;
        if (wall.backSector >= 0) counts[wall.backSector]++;
    }

    int bestSector = -1;
    int bestCount = -1;

    for (const auto& [sectorId, count] : counts) {
        if (count > bestCount) {
            bestCount = count;
            bestSector = sectorId;
        }
    }

    return bestSector;
}

static std::unique_ptr<BSPNode> MakeSubsectorLeaf(const std::vector<Wall>& walls) {
    auto leaf = std::make_unique<BSPNode>();
    leaf->isLeaf = true;
    leaf->subsector = std::make_unique<Subsector>();
    leaf->subsector->walls = walls;
    leaf->subsector->sectorId = ChooseDominantSector(walls);
    leaf->sectorId = leaf->subsector->sectorId;
    return leaf;
}

std::unique_ptr<BSPNode> BSPNode::BuildTree(const std::vector<Wall>& walls) {
    if (walls.empty()) return nullptr;

    // First-pass subsector stopping rule.
    // This is not a full Doom subsector builder, but it gives real leaf wall sets.
    if (walls.size() <= MAX_SUBSECTOR_WALLS) {
        return MakeSubsectorLeaf(walls);
    }

    auto node = std::make_unique<BSPNode>();

    size_t splitterIndex = ChooseBestSplitter(walls);
    node->partition = walls[splitterIndex];
    node->coplanarWalls.push_back(walls[splitterIndex]);

    std::vector<Wall> frontWalls;
    std::vector<Wall> backWalls;

    for (size_t i = 0; i < walls.size(); i++) {
        if (i == splitterIndex) continue;

        const Wall& current = walls[i];

        switch (ClassifyWall(node->partition, current)) {
            case WallClassification::Coplanar:
                node->coplanarWalls.push_back(current);
                break;

            case WallClassification::Front:
                frontWalls.push_back(current);
                break;

            case WallClassification::Back:
                backWalls.push_back(current);
                break;

            case WallClassification::Spanning: {
                Wall frontPiece;
                Wall backPiece;
                SplitWall(node->partition, current, frontPiece, backPiece);

                if (!IsTinyFragment(frontPiece)) {
                    frontWalls.push_back(frontPiece);
                }

                if (!IsTinyFragment(backPiece)) {
                    backWalls.push_back(backPiece);
                }
                break;
            }
        }
    }

    if (!frontWalls.empty()) {
        node->front = BuildTree(frontWalls);
    } else {
        node->front = std::make_unique<BSPNode>();
        node->front->isLeaf = true;
        node->front->subsector = std::make_unique<Subsector>();
        node->front->subsector->sectorId = node->partition.frontSector;
        node->front->sectorId = node->partition.frontSector;
    }

    if (!backWalls.empty()) {
        node->back = BuildTree(backWalls);
    } else {
        node->back = std::make_unique<BSPNode>();
        node->back->isLeaf = true;
        node->back->subsector = std::make_unique<Subsector>();
        node->back->subsector->sectorId = node->partition.backSector;
        node->back->sectorId = node->partition.backSector;
    }

    return node;
}

void TraverseTree(const BSPNode* node, const Vector2& playerPos, std::vector<Wall>& outWalls) {
    if (!node) return;

    if (node->isLeaf) {
        if (node->subsector) {
            for (const Wall& wall : node->subsector->walls) {
                outWalls.push_back(wall);
            }
        }
        return;
    }

    float side = PointSide(node->partition, playerPos);

    if (side >= 0) {
        TraverseTree(node->front.get(), playerPos, outWalls);

        for (const Wall& wall : node->coplanarWalls) {
            outWalls.push_back(wall);
        }

        TraverseTree(node->back.get(), playerPos, outWalls);
    }
    else {
        TraverseTree(node->back.get(), playerPos, outWalls);

        for (const Wall& wall : node->coplanarWalls) {
            outWalls.push_back(wall);
        }

        TraverseTree(node->front.get(), playerPos, outWalls);
    }
}

int FindPlayerSector(const BSPNode* node, const Vector2& playerPos) {
    if (!node) return -1;
    if (node->isLeaf) return node->sectorId;

    float side = PointSide(node->partition, playerPos);

    if (side >= 0) return FindPlayerSector(node->front.get(), playerPos);
    return FindPlayerSector(node->back.get(), playerPos);
}

const Subsector* FindPlayerSubsector(const BSPNode* node, const Vector2& playerPos) {
    if (!node) return nullptr;
    if (node->isLeaf) return node->subsector.get();

    float side = PointSide(node->partition, playerPos);

    if (side >= 0) return FindPlayerSubsector(node->front.get(), playerPos);
    return FindPlayerSubsector(node->back.get(), playerPos);
}