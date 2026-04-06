//
// Created by berke on 4/6/2026.
//

#include "../../Headers/Renderer/MapEditor.h"

namespace {
    inline std::vector<Wall> walls;
    inline std::vector<Sector> sectors;
}

namespace MapEditor {
    void CreateWall(const Vector2 start, const Vector2 end) {
        walls.push_back({start, end});
    }

    void CreateSectorDirectly(const Sector s) {
        sectors.push_back(s);
    }
    void CreateWallDirectly(const Wall w) {
        walls.push_back(w);
    }
}
