//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_WALL_H
#define TILKYENGINE_WALL_H

#include "../Math/Vector.h"

struct Wall {
    Vector2 start, end;
    Vector3 color;
    int frontSector = -1, backSector = -1;
};

#endif //TILKYENGINE_WALL_H