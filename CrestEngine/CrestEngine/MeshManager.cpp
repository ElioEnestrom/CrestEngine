
#include "MeshManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
#include <thread>
#include <mutex>

MeshManager* MeshManager::instance = nullptr;

MeshManager::MeshManager() {

}

void MeshManager::Allocate() {
	if (instance != nullptr)
		return;

	instance = new MeshManager();
}

MeshManager& MeshManager::Get() {
	return *instance;
}

Mesh* MeshManager::loadOBJ(const std::string& filename)
{
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> positionIndex;
    std::vector<unsigned int> textureIndex;
    std::vector<unsigned int> normalIndex;
    std::vector<unsigned int> vertexIndex;


    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return nullptr;
    }

    Mesh* newMesh = new Mesh();
    newMesh->path = filename;

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
            //std::cout << vertices[2] << std::endl;
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
            unsigned int posIdx, texIdx, normIdx;
            char slash; // Skip the slashes
            for (int i = 0; i < 3; ++i) {
                iss >> posIdx >> slash >> texIdx >> slash >> normIdx;
                positionIndex.push_back(posIdx - 1);  // Convert to 0-based index
                textureIndex.push_back(texIdx - 1);
                normalIndex.push_back(normIdx - 1);

                //std::cout << posIdx << std::endl;
            }
            /*else if (prefix == "f") {
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
            }*/

        }
    }
    Vertex vertex;
    for (size_t i = 0; i < positionIndex.size(); i++) {
        vertex.position = glm::vec3(
            vertices[3 * positionIndex[i]],
            vertices[3 * positionIndex[i] + 1],
            vertices[3 * positionIndex[i] + 2]
        );
        vertex.texCoord = glm::vec2(
            texCoords[2 * textureIndex[i]],
            texCoords[2 * textureIndex[i] + 1]
        );
        newMesh->vertices.push_back(vertex);
        //finalVertices.push_back(vertex);
        //                finalVertices.push_back(vertex);
    }

    //for (size_t i = 0; i < normalIndex.size(); ++i) {
    //    vertex.normal = glm::vec3(
    //        normals[3 * normalIndex[i]],
    //        normals[3 * normalIndex[i] + 1],
    //        normals[3 * normalIndex[i] + 2]
    //    );
    //}

//for (size_t i = 0; i < finalVertices.size(); ++i)
//{
//	std::cout << finalVertices[i].position.x << std::endl;
//	std::cout << finalVertices[i].position.y << std::endl;
//	std::cout << finalVertices[i].position.z << std::endl;
//    
//}


    //std::cout << vertices[3 * positionIndex[0]] << std::endl;
    //std::cout << vertices[3 * positionIndex[1] + 1] << std::endl;
    //std::cout << vertices[3 * positionIndex[2] + 2] << std::endl;
    //std::cout << finalVertices.size() << std::endl;
    //for (size_t i = 0; i < textureIndex.size(); ++i) 
    //{
    //}
    //for (size_t i = 0; i < normalIndex.size(); ++i) 
    //{
    //
    //}

    for (size_t i = 0; i < positionIndex.size(); ++i) {
        vertexIndex.push_back(static_cast<unsigned int>(positionIndex[i]));
        vertexIndex.push_back(static_cast<unsigned int>(textureIndex[i]));
        vertexIndex.push_back(static_cast<unsigned int>(normalIndex[i]));
    }
    MeshManager::Get().meshList.push_back(newMesh);
    return newMesh;
}

Mesh* MeshManager::GetMesh(std::string filename)
{
    return nullptr;
}
