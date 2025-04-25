#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Entities.h"
#include "Level.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include <map>


void ImguiManager::UpdateImGui(
    bool drawCubes,
    EntityManager& entityManager,
    Entity*& currentlySelected,
    std::vector<std::string>& modelNames,
    int& currentModelIndex,
    std::vector<Vertex>& currentObject,
    std::unordered_map<std::string, Mesh*>& meshMap,
    unsigned int& currentBuffer,
    std::unordered_map<unsigned int, unsigned int>& VAOs,
    std::vector<std::string>& textureNames,
    int& texture1Index,
    int& texture2Index,
    float& textureMixer,
    bool& normalView,
    Shader& ourShader)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // "Cubes!" UI
    ImGui::SetNextWindowPos(ImVec2(0, 0)); // Top-left corner
    ImGui::SetNextWindowSize(ImVec2(350, ImGui::GetIO().DisplaySize.y / 1.5f)); // Half screen height
    ImGui::Begin("Cubes!", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // General Settings
    ImGui::Text("General Settings");
    ImGui::Separator();
    ImGui::Checkbox("Draw Cubes", &drawCubes);
    ImGui::Checkbox("View Normals", &normalView);

    ImGui::Spacing();

    // Entity Actions
    ImGui::Text("Entity Actions");
    ImGui::Separator();
    if (ImGui::Button("Spawn Object")) {
        Entity* newEntity = entityManager.CreateEntity();
        newEntity->name = "Entity " + std::to_string(entityManager.amountOfObjects);
		entityManager.amountOfObjects++;
        currentlySelected = newEntity;
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete Object")) {
        if (currentlySelected && !currentlySelected->isDirectionalLight) {

            entityManager.DeleteEntity(currentlySelected);
            currentlySelected = nullptr;
        }
        else {
            std::cout << "No entity selected to delete." << std::endl;
        }
    }
    if (ImGui::Button("Spawn Light Source")) {
        Entity* newEntity = entityManager.CreateEntity();
        newEntity->objectShaderType = LIGHT_SOURCE_SHADER;
        newEntity->position = glm::vec3(0, 3.0f, -20.0f);
        newEntity->entityPosition = glm::vec3(0, 3.0f, -20.0f);
        newEntity->name = "Light Source " + std::to_string(entityManager.lightSources.size());
       entityManager.lightSources.push_back(newEntity);
        currentlySelected = newEntity;
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Light")) {
        Entity* newEntity = entityManager.CreateEntity();
        newEntity->objectShaderType = LIGHTING_SHADER;
        newEntity->name = "Light " + std::to_string(entityManager.amountOfLights);
		entityManager.amountOfLights++;

        currentlySelected = newEntity;
    }
    if (ImGui::Button("Load Level")) {
        Level level;
        level.LoadLevel(entityManager);
        currentlySelected = entityManager.entities.front();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Level")) {
        Level level;
        level.SaveLevel(entityManager);
    }

    ImGui::Spacing();

    // Scene View
    ImGui::Text("Scene View");
    ImGui::Separator();
    if (ImGui::BeginListBox("##SceneView", { 160, 200 })) {
        for (Entity* entity : entityManager.entities) {
            bool isSelected = entity == currentlySelected;
            if (ImGui::Selectable(entity->name.c_str(), isSelected)) {
                currentlySelected = entity;
            }
        }
        ImGui::EndListBox();
    }

    ImGui::Spacing();

    // Entity Details
    if (currentlySelected) {
        ImGui::Text("Entity Details");
        ImGui::Separator();

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

        ImGui::Spacing();

        // Model Selection
        const char* comboPreviewValue = modelNames[currentModelIndex].c_str();
        if (ImGui::BeginCombo("Select Model", comboPreviewValue)) {
            for (int n = 0; n < modelNames.size(); n++) {
                const bool isSelected = (currentModelIndex == n);
                if (ImGui::Selectable(modelNames[n].c_str(), isSelected)) {
                    currentModelIndex = n;
                    currentlySelected->model = modelNames[n];
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Spacing();

        // Texture Selection
        const char* texture1Preview = textureNames[currentlySelected->textureIndex1].c_str();
        if (ImGui::BeginCombo("Texture 1", texture1Preview)) {
            for (int n = 0; n < textureNames.size(); n++) {
                const bool isSelected = (currentlySelected->textureIndex1 == n);
                if (ImGui::Selectable(textureNames[n].c_str(), isSelected)) {
                    currentlySelected->textureIndex1 = n;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        const char* texture2Preview = textureNames[currentlySelected->textureIndex2].c_str();
        if (ImGui::BeginCombo("Texture 2", texture2Preview)) {
            for (int n = 0; n < textureNames.size(); n++) {
                const bool isSelected = (currentlySelected->textureIndex2 == n);
                if (ImGui::Selectable(textureNames[n].c_str(), isSelected)) {
                    currentlySelected->textureIndex2 = n;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SliderFloat("Texture Mixer", &currentlySelected->textureMixer, 0.0f, 1.0f);

        ImGui::Text("Mipmap Settings");
        ImGui::Separator();

        static int currentMipmapSetting = 0; // Default to the first setting
        const char* mipmapOptions[] = {
            "NEAREST",
            "LINEAR",
            "NEAREST_MIPMAP_NEAREST",
            "LINEAR_MIPMAP_NEAREST",
            "NEAREST_MIPMAP_LINEAR",
            "LINEAR_MIPMAP_LINEAR"
        };

        if (ImGui::BeginCombo("Mipmap Setting", mipmapOptions[currentMipmapSetting])) {
            for (int n = 0; n < IM_ARRAYSIZE(mipmapOptions); n++) {
                const bool isSelected = (currentMipmapSetting == n);
                if (ImGui::Selectable(mipmapOptions[n], isSelected)) {
                    currentMipmapSetting = n;

                    // Apply the selected mipmap setting to the texture
                    Texture::MipmapSetting setting = static_cast<Texture::MipmapSetting>(n);
                    Texture::switchMipmapSetting(currentlySelected->textureIndex1, setting);
                    Texture::switchMipmapSetting(currentlySelected->textureIndex2, setting);
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Spacing();

		if (currentlySelected->objectShaderType == LIGHT_SOURCE_SHADER) {
			ImGui::Text("Light Settings");
			ImGui::Separator();
			ImGui::SliderFloat("Ambient", &currentlySelected->ambient, 0.0f, 3.0f);
			ImGui::SliderFloat("Diffuse", &currentlySelected->diffuse, 0.0f, 3.0f);
			ImGui::SliderFloat("Specular", &currentlySelected->specular, 0.0f, 3.0f);
			//ImGui::SliderFloat("Constant", &currentlySelected->constant, 0.0f, 1.0f);
			//ImGui::SliderFloat("Linear", &currentlySelected->linear, 0.0f, 1.0f);
			//ImGui::SliderFloat("Quadratic", &currentlySelected->quadratic, 0.0f, 1.0f);
		}
        else if (currentlySelected->objectShaderType == OBJECT_SHADER) {
            ImGui::Spacing();
            // Collider Settings
            const char* colliderType = currentlySelected->myCollider ? currentlySelected->myCollider->GetTypeName() : "None";
            if (ImGui::BeginCombo("Collider Type", colliderType)) {
                if (currentlySelected->myCollider) {
                    if (ImGui::Selectable("Sphere", strcmp(colliderType, "SphereCollider") == 0)) {
                        currentlySelected->CreateSphereCollider(1.0f);
                    }
                    if (ImGui::Selectable("Box", strcmp(colliderType, "BoxCollider") == 0)) {
                        currentlySelected->CreateBoxCollider(glm::vec3(1.0f, 1.0f, 1.0f));
                    }
                }
                ImGui::EndCombo();
            }

            if (currentlySelected->myCollider) {
                ImGui::Checkbox("Is Kinematic", &currentlySelected->myCollider->isKinematic);
                if (!currentlySelected->myCollider->isKinematic) {
                    ImGui::SameLine();
                    ImGui::Checkbox("Has Gravity", &currentlySelected->myCollider->hasGravity);
                }
            }
            ImGui::Checkbox("View Container Specular Map", &currentlySelected->viewContainerSpecularMap);
        }
    }

    ImGui::End();

    // "Camera Controller" UI
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y / 1.5f)); // Bottom-left corner
    ImGui::SetNextWindowSize(ImVec2(300, 200)); // Fixed size
    ImGui::Begin("Camera Controller", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Camera Settings");
    ImGui::Separator();

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
}
