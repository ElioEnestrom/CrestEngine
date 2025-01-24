#pragma once
#include "Mesh.h"
#include "MessageHandler.h"
#include <list>

class MeshManager {

	MeshManager();

public:
	static void Allocate();
	static MeshManager& Get();
	void ProcessMessage(Message* message);

	static void* loadOBJ(const std::string& filename);
	Mesh* GetMesh(std::string filename);

	std::list<Mesh*> meshList;

	unsigned int id;
private:
	static MeshManager* instance;
};