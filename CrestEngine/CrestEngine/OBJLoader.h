#pragma once
#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <vector>
#include <string>

namespace OBJLoader {
    // Function declaration
    void loadOBJ(const std::string& filename,
        std::vector<float>& vertices,
        std::vector<float>& normals,
        std::vector<float>& texCoords);
}

#endif // LOAD_OBJ_H
