#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>

struct Vertex {
    glm::vec3 position;
    float x, y, z; // alt. 2
};

struct Mesh {
    std::list<Vertex*> vertices;
};
