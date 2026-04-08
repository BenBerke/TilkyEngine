//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_WALL_H
#define TILKYENGINE_WALL_H

#include "../Math/Vector.h"

struct Wall {
    Vector2 start, end;
    Vector3 color;

    Vector2 dir, normal;
    float lengthSq;
    Wall(const Vector2& start, const Vector2& end, const Vector3& color)
           : start(start), end(end), color(color) {
        dir = end - start;
        lengthSq = dir.Dot(dir);

        normal = {-dir.y, dir.x};
        if (lengthSq > 0.00001f) {
            normal.Normalize();
        }
    }
};

#endif //TILKYENGINE_WALL_H