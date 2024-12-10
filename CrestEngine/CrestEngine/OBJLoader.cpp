#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Mesh.h"


namespace OBJLoader {

    void loadOBJ(const std::string& filename,
        std::vector<Vertex>& vertices,
        std::vector<float>& normals,
        std::vector<float>& texCoords) 
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                Vertex vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                vertices.push_back(vertex);
            }
            else if (prefix == "vn") {
                float nx, ny, nz;
                iss >> nx >> ny >> nz;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
            else if (prefix == "vt") {
                float u, v;
                iss >> u >> v;
                texCoords.push_back(u);
                texCoords.push_back(v);
            }
        }
    }

} // namespace OBJLoader
