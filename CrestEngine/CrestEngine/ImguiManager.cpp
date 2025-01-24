#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Entities.h"
#include "Mesh.h"
#include <map>


void ImguiManager::UpdateImGui(
	bool drawCubes, 
	float* cubePosition, 
	float* cubeRotation, 
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
	float& textureMixer)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	ImGui::Begin("Cubes!");
	ImGui::Checkbox("Draw Cubes", &drawCubes);
	ImGui::DragFloat3("Position", cubePosition, 0.01f, -FLT_MAX, FLT_MAX);
	ImGui::DragFloat3("Rotation", cubeRotation, 0.01f, -FLT_MAX, FLT_MAX);


	if (ImGui::Button("Spawn Cube"))
	{
		// Create a new entity and set its position
		Entity* newEntity = entityManager.CreateEntity();
		newEntity->position = glm::vec3(0, -5.f, -20.f);

		//std::cout << &currentlySelected->name[0];
		//std::cout << std::to_string(entityManager.entities.size());
		//std::cout << newEntity->name.c_str();
	}
	//ImGui::InputText("Name", &currentlySelected->name[0], currentlySelected->name.size());
	ImGui::Text("Currently Selected: %s", currentlySelected ? currentlySelected->name.c_str() : "None");

	if (ImGui::BeginListBox("Scene View", { 80, 200 }))
	{
		for (Entity* entity : entityManager.entities)
		{
			bool isSelected = entity == currentlySelected;
			if (ImGui::Selectable(entity->name.c_str(), isSelected))
			{
				if (currentlySelected != entity)
				{
					cubePosition[0] = { entity->position.x };
					cubePosition[1] = { entity->position.y };
					cubePosition[2] = { entity->position.z };
					cubeRotation[0] = { entity->rotation.x };
					cubeRotation[1] = { entity->rotation.y };
					cubeRotation[2] = { entity->rotation.z };
				}
				currentlySelected = entity;
				//std::cout << currentlySelected->name.c_str();
			}
		}
		ImGui::EndListBox();
	}const char* comboPreviewValue = modelNames[currentModelIndex].c_str(); // Preview value
	if (ImGui::BeginCombo("Select Model", comboPreviewValue))
	{
		for (int n = 0; n < modelNames.size(); n++)
		{
			const bool isSelected = (currentModelIndex == n);
			if (ImGui::Selectable(modelNames[n].c_str(), isSelected))
			{
				currentModelIndex = n; // Update the current model index

				currentObject = meshMap[modelNames[n]]->vertices;
				currentBuffer = VAOs[meshMap[modelNames[n]]->id];

				// Load or switch to the selected model here
				std::cout << "Switched to model: " << modelNames[n] << std::endl;
			}

			// Set the initial focus when opening the combo
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::PushItemWidth(100);
	const char* texture1Preview = textureNames[texture1Index].c_str();
	if (ImGui::BeginCombo("Texture 1", texture1Preview))
	{
		for (int n = 0; n < textureNames.size(); n++)
		{
			const bool isSelected = (texture1Index == n);
			if (ImGui::Selectable(textureNames[n].c_str(), isSelected))
			{
				texture1Index = n;
				std::cout << "Switched Texture 1 to: " << textureNames[n] << std::endl;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine(); // Place the next combo box next to the previous one

	// Combo box for the second texture
	ImGui::PushItemWidth(100);
	const char* texture2Preview = textureNames[texture2Index].c_str();
	if (ImGui::BeginCombo("Texture 2", texture2Preview))
	{
		for (int n = 0; n < textureNames.size(); n++)
		{
			const bool isSelected = (texture2Index == n);
			if (ImGui::Selectable(textureNames[n].c_str(), isSelected))
			{
				texture2Index = n;
				std::cout << "Switched Texture 2 to: " << textureNames[n] << std::endl;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SliderFloat("Texture Mixer", &textureMixer, 0.0f, 1.0f);
	ImGui::End();

}
