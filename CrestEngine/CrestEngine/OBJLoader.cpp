#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Mesh.h"


namespace OBJLoader {

    void loadOBJ(const std::string& filename,
        std::vector<float>& vertices,
        std::vector<float>& normals,
        std::vector<float>& texCoords, 
        std::vector<unsigned int>& positionIndex, 
        std::vector<unsigned int>& textureIndex, 
        std::vector<unsigned int>& normalIndex)
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
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
                //Vertex vertex;
                //iss >> vertex.x >> vertex.y >> vertex.z;
                //vertices.push_back(vertex);
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
            else if (prefix == "f") {
                unsigned int p1, t1, n1, p2, t2, n2, p3, t3, n3; char slash; // to skip the slashes

                iss >> p1 >> slash >> t1 >> slash >> n1
                    >> p2 >> slash >> t2 >> slash >> n2
                    >> p3 >> slash >> t3 >> slash >> n3;

				//std::cout << p1 << std::endl;
				//std::cout << p2 << std::endl;
				//std::cout << p3 << std::endl;

				positionIndex.push_back(p1 - 1);
				positionIndex.push_back(p2 - 1);
				positionIndex.push_back(p3 - 1);
				textureIndex.push_back(t1);
				textureIndex.push_back(t2);
				textureIndex.push_back(t3);
				normalIndex.push_back(n1);
				normalIndex.push_back(n2);
				normalIndex.push_back(n3);
            }
        }
        for (size_t i = 0; i < positionIndex.size(); ++i) {
            Vertex vertex;
            vertex.position = glm::vec3(
                vertices[3 * positionIndex[i]],         // x
                vertices[3 * positionIndex[i] + 1],     // y
                vertices[3 * positionIndex[i] + 2]      // z
            );
            vertex.texCoord = glm::vec2(
                texCoords[2 * textureIndex[i]],         // u
                texCoords[2 * textureIndex[i] + 1]      // v
            );
            vertex.normal = glm::vec3(
                normals[3 * normalIndex[i]],            // nx
                normals[3 * normalIndex[i] + 1],        // ny
                normals[3 * normalIndex[i] + 2]         // nz
            );

            //finalVertices.push_back(vertex);
        }

    }

} // namespace OBJLoader
