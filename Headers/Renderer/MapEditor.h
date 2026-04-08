//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_MAPEDITOR_H
#define TILKYENGINE_MAPEDITOR_H

#include <vector>

#include "../Objects/Wall.h"


namespace MapEditor {
    inline std::vector<Wall> walls;
    void CreateWall(Vector2 start, Vector2 end);

    void CreateWallDirectly(Wall w);
}


#endif //TILKYENGINE_MAPEDITOR_H