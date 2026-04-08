//
// Created by berke on 4/6/2026.
//

#include "../../Headers/Renderer/MapEditor.h"

namespace {
    inline std::vector<Wall> walls;
}

namespace MapEditor {
    void CreateWallDirectly(const Wall w) {
        walls.push_back(w);
    }
}
