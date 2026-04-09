//
// Created by berke on 4/6/2026.
//

#ifndef TILKYENGINE_MAPEDITOR_H
#define TILKYENGINE_MAPEDITOR_H

#include <vector>

#include "../Objects/Wall.h"
#include "../Objects/Sector.h"


namespace MapEditor {
    inline std::vector<Wall> walls;
    inline std::vector<Sector> sectors;
    void CreateWall(Vector2 start, Vector2 end);

    void CreateWallDirectly(Wall w);
    void CreateSectorDirectly(Sector sector);
}


#endif //TILKYENGINE_MAPEDITOR_H