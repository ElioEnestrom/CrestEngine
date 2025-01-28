#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Entities.h"
#include "Mesh.h"
#include <map>


void ImguiManager::UpdateImGui(
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
	float& textureMixer)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	ImGui::Begin("Cubes!");
	ImGui::Checkbox("Draw Cubes", &drawCubes);

	if (ImGui::Button("Spawn Object"))
	{
		// Create a new entity and set its position
		Entity* newEntity = entityManager.CreateEntity();
		currentlySelected = newEntity;

		//std::cout << &currentlySelected->name[0];
		//std::cout << std::to_string(entityManager.entities.size());
		//std::cout << newEntity->name.c_str();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Object"))
	{
		if (currentlySelected)
		{
			entityManager.DeleteEntity(currentlySelected);
			currentlySelected = nullptr;
		}
		else
		{
			std::cout << "No entity selected to delete." << std::endl;
		}
	}
	//ImGui::InputText("Name", &currentlySelected->name[0], currentlySelected->name.size());
	ImGui::Text("Currently Selected: %s", currentlySelected ? currentlySelected->name.c_str() : "None");

	if (currentlySelected) {

		if (ImGui::BeginListBox("Scene View", { 80, 200 }))
		{
			for (Entity* entity : entityManager.entities)
			{
				bool isSelected = entity == currentlySelected;
				if (ImGui::Selectable(entity->name.c_str(), isSelected))
				{
					currentlySelected = entity;
					//std::cout << currentlySelected->name.c_str();
				}
			}
			ImGui::EndListBox();
		}



		ImGui::InputText("Name", &currentlySelected->name[0], currentlySelected->name.size() + 1);
		ImGui::DragFloat3("Position", currentlySelected->entityPosition, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Rotation", currentlySelected->entityRotation, 0.01f, -FLT_MAX, FLT_MAX);

		const char* comboPreviewValue = modelNames[currentModelIndex].c_str(); // Preview value
		if (ImGui::BeginCombo("Select Model", comboPreviewValue))
		{
			for (int n = 0; n < modelNames.size(); n++)
			{
				const bool isSelected = (currentModelIndex == n);
				if (ImGui::Selectable(modelNames[n].c_str(), isSelected))
				{
					currentModelIndex = n; // Update the current model index

					if (currentlySelected)
					{
						currentlySelected->model = modelNames[n]; // Update the model of the currently selected entity
					}
					else
					{
						std::cout << "No entity selected to change model." << std::endl;
					}

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
		const char* texture1Preview = textureNames[currentlySelected->textureIndex1].c_str();
		if (ImGui::BeginCombo("Texture 1", texture1Preview))
		{
			for (int n = 0; n < textureNames.size(); n++)
			{
				const bool isSelected = (currentlySelected->textureIndex1 == n);
				if (ImGui::Selectable(textureNames[n].c_str(), isSelected))
				{
					currentlySelected->textureIndex1 = n;
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
		const char* texture2Preview = textureNames[currentlySelected->textureIndex2].c_str();
		if (ImGui::BeginCombo("Texture 2", texture2Preview))
		{
			for (int n = 0; n < textureNames.size(); n++)
			{
				const bool isSelected = (currentlySelected->textureIndex2 == n);
				if (ImGui::Selectable(textureNames[n].c_str(), isSelected))
				{
					currentlySelected->textureIndex2 = n;
					std::cout << "Switched Texture 2 to: " << textureNames[n] << std::endl;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SliderFloat("Texture Mixer", &currentlySelected->textureMixer, 0.0f, 1.0f);
	}
	
	ImGui::End();

}
