#pragma once
#include "Interface.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <list>

struct Vertex {
    glm::vec3 position;  // x, y, z
    glm::vec2 texCoord;  // u, v
    glm::vec3 normal;    // nx, ny, nz
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::string path;
    unsigned int id;


    bool Serialize(std::fstream& stream, const std::vector<unsigned int> object);

    bool Deserialize(std::fstream& stream);

    void DeleteSerializedFiles();
};
