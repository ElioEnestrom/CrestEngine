#pragma once
#include "Mesh.h"
#include "MessageHandler.h"
#include <list>
#include <mutex>

class MeshManager {

	MeshManager();

public:
	static void Allocate();
	static MeshManager& Get();
	void ProcessMessage(Message* message);

	static void* loadOBJ(const std::string& filename);
	Mesh* GetMesh(std::string filename);

	std::list<Mesh*> meshList;
	std::vector<Message> objMessages = {
		{MessageType::Object, "Flag.obj"},
		{MessageType::Object, "Viking_House.obj"},
		{MessageType::Object, "Cube.obj"},
		{MessageType::Object, "MySphere.obj"}
	};

	unsigned int id;
private:
	std::mutex meshMutex;
	static MeshManager* instance;
};