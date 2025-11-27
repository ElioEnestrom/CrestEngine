#pragma once
#include "Entities.h"
#include "Mesh.h"
#include "Shader.h"
#include <vector>
#include <unordered_map>
#include <string>
#include "RenderingTypes.h"

class ImguiManager
{
public:
	void UpdateImGui(
		bool drawCubes,
		EntityManager& entityManager,
		std::vector<std::string>& modelNames,
		std::vector<Vertex>& currentObject,
		std::unordered_map<std::string,
		Mesh*>& meshMap,
		unsigned int& currentBuffer,
		std::unordered_map<unsigned int,
		unsigned int>& VAOs,
		std::vector<std::string>& textureNames,
		bool& normalView,
		Shader& ourShader);

	void InitializeImGui(WindowContext context);

	int currentModelIndex = 0;
	int texture1Index = 0;
	int texture2Index = 1;
	float textureMixer = 0.35f;
	Entity* currentlySelected = nullptr;

	//ImguiManager();
	//~ImguiManager();
	//void Initialize();
	//void Update();
	//void Render();
	//void Shutdown();
};