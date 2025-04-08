#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Entities.h"
#include "Level.h"
#include "Mesh.h"
#include "Camera.h"
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
	float& textureMixer,
	bool& normalView)
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
	ImGui::SameLine();
	ImGui::Checkbox("View Normals", &normalView);
	if (ImGui::Button("Spawn Light Source"))
	{
		Entity* newEntity = entityManager.CreateEntity();
		newEntity->objectShaderType = LIGHT_SOURCE_SHADER;
		newEntity->position = glm::vec3(0, 3.0f, -20.0f);
		newEntity->entityPosition = glm::vec3(0, 3.0f, -20.0f);
		currentlySelected = newEntity;
	}
	ImGui::SameLine();
	if (ImGui::Button("Spawn Light"))
	{
		Entity* newEntity = entityManager.CreateEntity();
		newEntity->objectShaderType = LIGHTING_SHADER;
		currentlySelected = newEntity;
	}
	if (ImGui::Button("LoadLevel"))
	{
		Level level;
		level.LoadLevel(entityManager);
		currentlySelected = nullptr;
		//Level::Get().LoadLevel(entityManager);

	}
	ImGui::SameLine();
	if (ImGui::Button("Save Level"))
	{
		Level level;
		level.SaveLevel(entityManager);
		//Level::Get().SaveLevel(entityManager);
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
				currentlySelected = entity;
				//std::cout << currentlySelected->name.c_str();
			}
		}
		ImGui::EndListBox();
	}

	if (currentlySelected) {

		float entityPosition[3] = { currentlySelected->entityPosition[0], currentlySelected->entityPosition[1], currentlySelected->entityPosition[2] };
		float entityRotation[3] = { currentlySelected->entityRotation[0], currentlySelected->entityRotation[1], currentlySelected->entityRotation[2] };
		float entityScale[3] = { currentlySelected->entityScale[0], currentlySelected->entityScale[1], currentlySelected->entityScale[2] };

		ImGui::InputText("Name", &currentlySelected->name[0], currentlySelected->name.size() + 1);
		ImGui::DragFloat3("Position", entityPosition, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Rotation", entityRotation, 0.3f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Scale", entityScale, 0.01f, -FLT_MAX, FLT_MAX);
		
		currentlySelected->entityPosition = glm::vec3(entityPosition[0], entityPosition[1], entityPosition[2]);
		currentlySelected->entityRotation = glm::vec3(entityRotation[0], entityRotation[1], entityRotation[2]);
		currentlySelected->entityScale = glm::vec3(entityScale[0], entityScale[1], entityScale[2]);


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

		ImGui::SameLine(); 

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


		const char* colliderType = currentlySelected->myCollider ? currentlySelected->myCollider->GetTypeName() : "None";
		if (ImGui::BeginCombo("Collider Type", colliderType))
		{
			if (currentlySelected->myCollider)
			{
				if (ImGui::Selectable("Sphere", strcmp(colliderType, "SphereCollider") == 0))
				{
					currentlySelected->CreateSphereCollider(1.0f);
				}
				if (ImGui::Selectable("Box", strcmp(colliderType, "BoxCollider") == 0))
				{
					currentlySelected->CreateBoxCollider(glm::vec3(1.0f, 1.0f, 1.0f));
				}
			}
			ImGui::EndCombo();
		}


		if (currentlySelected->myCollider) {
			ImGui::Checkbox("Is Kinematic", &currentlySelected->myCollider->isKinematic);
			if (!currentlySelected->myCollider->isKinematic) 
			{
				ImGui::SameLine();
				ImGui::Checkbox("Has Gravity", &currentlySelected->myCollider->hasGravity);
			}
		}
	}
	
	ImGui::End();

	ImGui::Begin("Camera Controller");

	glm::vec3 cameraPos = Camera::GetCameraPos();
	float cameraPosArray[3] = { cameraPos.x, cameraPos.y, cameraPos.z };

	ImGui::DragFloat3("Position", cameraPosArray, 0.01f, -FLT_MAX, FLT_MAX);

	Camera::SetCameraPos(glm::vec3(cameraPosArray[0], cameraPosArray[1], cameraPosArray[2]));
	
	glm::vec2 cameraRotation = Camera::GetCameraRotation();
	float cameraRotationArray[2] = { cameraRotation.x, cameraRotation.y };

	ImGui::DragFloat("Yaw", &cameraRotationArray[0], 0.1f, -360.0f, 360.0f);
	ImGui::DragFloat("Pitch", &cameraRotationArray[1], 0.1f, -89.0f, 89.0f);
	Camera::SetCameraRotation(cameraRotationArray[0], cameraRotationArray[1]);

	ImGui::End();

	//Add another Imgui Window
	
}
