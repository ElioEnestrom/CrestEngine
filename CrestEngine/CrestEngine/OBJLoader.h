#pragma once
#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <vector>
#include <string>
#include "Mesh.h"

namespace OBJLoader {
    // Function declaration
    void loadOBJ(const std::string& filename,
        std::vector<float>& vertices,
        std::vector<float>& normals,
        std::vector<float>& texCoords,
        std::vector<unsigned int>& positionIndex,
        std::vector<unsigned int>& textureIndex,
        std::vector<unsigned int>& normalIndex,
        std::vector<unsigned int>& vertexIndex,
        std::vector<Vertex>& finalVertices);
}

#endif // LOAD_OBJ_H
