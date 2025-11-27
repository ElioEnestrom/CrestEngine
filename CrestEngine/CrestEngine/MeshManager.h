#pragma once
#include "Mesh.h"
#include "MessageHandler.h"
#include <list>
#include <mutex>
#include <condition_variable>
#include "MessageQueue.h"

class MeshManager {

	MeshManager();

public:
	static void Allocate();
	static MeshManager& Get();
	void ProcessMessage();
	void InitializeAndLoadMeshes();

	static void loadOBJ(const std::string& filename);
	void AddToMeshList(Mesh* mesh);
	void WaitForMeshLoadingComplete();
	Mesh* GetMesh(std::string filename);

	std::list<Mesh*> meshList;
	std::vector<Message> objMessages = {
		{MessageType::Object, "Sphere.obj"},
		{MessageType::Object, "Flag.obj"},
		{MessageType::Object, "Viking_House.obj"},
		{MessageType::Object, "Cube2.obj"}
	};

	bool loadingComplete = false;
	unsigned int id;
private:
	std::mutex meshMutex;
	std::condition_variable loadingCV;
	static MeshManager* instance;
};