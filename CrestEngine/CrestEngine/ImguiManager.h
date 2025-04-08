#pragma once
#include "Entities.h"
#include "Mesh.h"
#include <vector>
#include <unordered_map>
#include <string>

class ImguiManager
{
public:
	void UpdateImGui(
		bool drawCubes,
		EntityManager& entityManager,
		Entity*& currentlySelected,
		std::vector<std::string>& modelNames,
		int& currentModelIndex,
		std::vector<Vertex>& currentObject,
		std::unordered_map<std::string,
		Mesh*>& meshMap,
		unsigned int& currentBuffer,
		std::unordered_map<unsigned int,
		unsigned int>& VAOs,
		std::vector<std::string>& textureNames,
		int& texture1Index,
		int& texture2Index,
		float& textureMixer,
		bool& normalView);

	//ImguiManager();
	//~ImguiManager();
	//void Initialize();
	//void Update();
	//void Render();
	//void Shutdown();
};