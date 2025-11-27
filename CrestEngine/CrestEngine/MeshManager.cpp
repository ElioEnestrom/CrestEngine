#include "MeshManager.h"
#include "MemoryChecker.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
#include <thread>
#include <filesystem>
#include <future>

MeshManager* MeshManager::instance = nullptr;
MemoryChecker memoryChecker;
unsigned int id;

MeshManager::MeshManager() {
	id = 0;
}

void MeshManager::Allocate() {
	if (instance != nullptr)
		return;

	instance = new MeshManager();
}

MeshManager& MeshManager::Get() {
	return *instance;
}

//void MeshManager::ProcessMessage()
//{
//    std::vector<std::thread> meshThreads;
//    for (int i = 0; i < objMessages.size(); i++) {
//        meshThreads.emplace_back([this, i]() {
//            loadOBJ(objMessages[i].msg); 
//        });
//    }
//    for (auto& t : meshThreads)
//    {
//        t.join();
//    }
//    return;
//    //std::cerr << "Unknown message: " << msg << std::endl;
//}

void MeshManager::loadOBJ(const std::string& filename)
{
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> positionIndex;
    std::vector<unsigned int> textureIndex;
    std::vector<unsigned int> normalIndex;
    std::vector<unsigned int> vertexIndex;

    std::fstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return;
    }

    std::fstream deSerializeFile(filename + ".binaryThingy", std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return;
    }
        
    Mesh* deserializeMesh = new Mesh();
    if (deserializeMesh->Deserialize(deSerializeFile)) {
        std::cerr << "Successfully deserialized mesh from file: " << filename << std::endl;
		MeshManager::Get().AddToMeshList(deserializeMesh);
        return;
    }

    //Reset the file stream to the beginning for OBJ parsing
    file.clear();
    file.seekg(0, std::ios::beg);

    // Calculating the estimated size of the object
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::end);
    fileSize = file.tellg() - fileSize;
    file.seekg(0, std::ios::beg);

	// Check if there is enough memory available (Divides by a million since statex.ullAvailPhys is in MB)
    if (!memoryChecker.CheckMemory(static_cast<float>(fileSize)/ 1000000)) {
        std::cerr << "Not enough memory to load OBJ file: " << filename << std::endl;
		std::cerr << "File size: " << fileSize << " bytes" << std::endl;
        return;
    }
	memoryChecker.CheckMemory();

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
            std::vector<unsigned int> facePositionIndices;
            std::vector<unsigned int> faceTextureIndices;
            std::vector<unsigned int> faceNormalIndices;
            unsigned int posIdx, texIdx, normIdx;
            char slash; // Skip the slashes
            while (iss >> posIdx >> slash >> texIdx >> slash >> normIdx) {
                facePositionIndices.push_back(posIdx - 1);  // Convert to 0-based index
                faceTextureIndices.push_back(texIdx - 1);
                faceNormalIndices.push_back(normIdx - 1);
            }
            // Triangulate the face (assuming it's a convex polygon)
            if (!facePositionIndices.empty()) {

                for (size_t i = 1; i < facePositionIndices.size() - 1; ++i) {
                    positionIndex.push_back(facePositionIndices[0]);
                    positionIndex.push_back(facePositionIndices[i]);
                    positionIndex.push_back(facePositionIndices[i + 1]);
        
                    textureIndex.push_back(faceTextureIndices[0]);
                    textureIndex.push_back(faceTextureIndices[i]);
                    textureIndex.push_back(faceTextureIndices[i + 1]);
        
                    normalIndex.push_back(faceNormalIndices[0]);
                    normalIndex.push_back(faceNormalIndices[i]);
                    normalIndex.push_back(faceNormalIndices[i + 1]);
                }
            }
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
		vertex.normal = glm::vec3(
			normals[3 * normalIndex[i]],
			normals[3 * normalIndex[i] + 1],
			normals[3 * normalIndex[i] + 2]
		);
        newMesh->vertices.push_back(vertex);
    }
    std::cout << std::endl;
    for (size_t i = 0; i < positionIndex.size(); ++i) {
        vertexIndex.push_back(static_cast<unsigned int>(positionIndex[i]));
        vertexIndex.push_back(static_cast<unsigned int>(textureIndex[i]));
        vertexIndex.push_back(static_cast<unsigned int>(normalIndex[i]));
    }

	
	MeshManager::Get().AddToMeshList(newMesh);

    std::fstream serializeFile(filename + ".binaryThingy", std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return;
    }
	newMesh->Serialize(serializeFile, vertexIndex);
}

void MeshManager::AddToMeshList(Mesh* mesh) {
	std::lock_guard<std::mutex> lock(meshMutex);

    MeshManager::Get().meshList.push_back(mesh);
    mesh->id = MeshManager::Get().id;
    MeshManager::Get().id++;

	if (meshList.size() == objMessages.size()) {
		loadingComplete = true;
		loadingCV.notify_all();  
		std::cout << "All meshes loaded successfully!" << std::endl;
	}
}

void MeshManager::WaitForMeshLoadingComplete() {
	std::unique_lock<std::mutex> lock(meshMutex);
	loadingCV.wait(lock, [this] { return loadingComplete; });
}

Mesh* MeshManager::GetMesh(std::string filename)
{
    return nullptr;
}

void MeshManager::InitializeAndLoadMeshes() {
	MessageQueue::Get().WorkerThreadStart(objMessages.size());

	for (int i = 0; i < objMessages.size(); i++) {
		MessageQueue::Get().QueueMessage(&objMessages[i]);
	}
    //MessageQueue::Get().WorkerThreadEnd();

	WaitForMeshLoadingComplete();
}
