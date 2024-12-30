#pragma once
#include "Mesh.h"
#include <list>

class MeshManager {

	MeshManager();

public:
	static void Allocate();
	static MeshManager& Get();

	static Mesh* loadOBJ(const std::string& filename);
	Mesh* GetMesh(std::string filename);

	std::list<Mesh*> meshList;
private:
	static MeshManager* instance;
};