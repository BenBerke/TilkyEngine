//
// Created by berke on 4/9/2026.
//

#ifndef TILKYENGINE_SECTOR_H
#define TILKYENGINE_SECTOR_H

#include "Wall.h"

struct Sector {
    float floorHeight, ceilHeight;
};

struct SubSector {
    std::vector<Wall> walls;
    std::vector<Vector2> polygon;
    int sectorId;
};

struct SectorPolygon {
    std::vector<Vector2> vertices;
    int sectorId;
};

#endif //TILKYENGINE_SECTOR_H