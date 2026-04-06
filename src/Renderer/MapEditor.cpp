//
// Created by berke on 4/6/2026.
//

#include "../../Headers/Renderer/MapEditor.h"

namespace {
    inline std::vector<Wall> walls;
}

namespace MapEditor {
    void CreateWall(Vector2 start, Vector2 end) {
        walls.push_back({start, end});
    }
}
