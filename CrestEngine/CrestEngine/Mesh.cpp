#include "Mesh.h"
#include <fstream>
#include <list>
#include <filesystem>

bool Mesh::Serialize(std::fstream& stream, std::vector<unsigned int> object)
{
    std::cerr << "Attempting to serialize mesh" << std::endl;
    if (!stream.is_open()) return false;

    std::cerr << "Serializing mesh: " << path << std::endl;

    // Serialize the id
    stream.write(reinterpret_cast<const char*>(&id), sizeof(id));

    // Serialize the path length and path
    size_t pathLength = path.size();
    stream.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
    stream.write(path.c_str(), pathLength);

    // Flush the stream to ensure data is written
    stream.flush();

    // Serialize the number of vertices
    size_t vertexCount = vertices.size();
    stream.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));

    // Serialize each vertex
    for (const auto& vertex : vertices) {
        stream.write(reinterpret_cast<const char*>(&vertex.position), sizeof(vertex.position));
        stream.write(reinterpret_cast<const char*>(&vertex.texCoord), sizeof(vertex.texCoord));
        stream.write(reinterpret_cast<const char*>(&vertex.normal), sizeof(vertex.normal));
    }

    // Flush the stream again
    stream.flush();

    return true;
}

bool Mesh::Deserialize(std::fstream& stream) 
{

    if (!stream.is_open()) return false;

    // Deserialize the id
    stream.read(reinterpret_cast<char*>(&id), sizeof(id));

    // Deserialize the path length and path
    size_t pathLength;
    stream.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
    path.resize(pathLength);
    stream.read(&path[0], pathLength);

    // Deserialize the number of vertices
    size_t vertexCount;
    stream.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));

    // Deserialize each vertex
    vertices.resize(vertexCount);
    for (auto& vertex : vertices) {
        stream.read(reinterpret_cast<char*>(&vertex.position), sizeof(vertex.position));
        stream.read(reinterpret_cast<char*>(&vertex.texCoord), sizeof(vertex.texCoord));
        stream.read(reinterpret_cast<char*>(&vertex.normal), sizeof(vertex.normal));
    }

    return true;
}

//void Mesh::DeleteSerializedFiles() {
//    if (!path.empty()) {
//        std::filesystem::path filePath(path);
//        if (std::filesystem::exists(filePath)) {
//            std::filesystem::remove(filePath);
//        }
//    }
//}