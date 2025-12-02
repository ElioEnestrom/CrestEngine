#include <iostream>
#include <cstring>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "OBJLoader.h"
#include "ImguiManager.h"
#include "PhysicsManager.h"
#include "Materials.h"

#include <thread>
#include <mutex>

#include "Shader.h"
#include "Entities.h"
#include "Camera.h"
#include "Time.h"
#include "Input.h"
#include "Texture.h"
#include "MeshManager.h"
#include "MessageHandler.h"
#include "MessageQueue.h"
#include <queue>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>

//#include <openxr/openxr.h>
//#include <openxr/openxr_platform.h>
#include "RenderingTypes.h"
#include "RenderingManager.h"
#include "VRManager.h"


//#include <DebugOutput.h>
//#include <GraphicsAPI_OpenGL.h>
//#include <OpenXRDebugUtils.h>


// Add Windows-specific includes for console handling
#ifdef _WIN32
#include <windows.h>
#include <csignal>

std::atomic<bool> g_shouldExit(false);

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_CLOSE_EVENT || signal == CTRL_C_EVENT) {
        g_shouldExit = true;
        // Give the program time to cleanup
        Sleep(2000);
        return TRUE;
    }
    return FALSE;
}
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void renderQuad();

const unsigned int SCREEN_WIDTH = 2568;
const unsigned int SCREEN_HEIGHT = 1440;



WindowContext InitializeWindow(); 
ShaderResources LoadShaders();
TextureResources LoadTextures();
RenderResources InitializeRenderResources();

void RenderEntity(Entity* entity, Shader& currentShader, 
	EntityManager& entityManager, const RenderContext& context,
	const glm::mat4& view, const glm::mat4& projection, 
	const glm::mat4& lightSpaceMatrix);

void SetupShaderLighting(Shader& shader, EntityManager& entityManager);
void ShouldLoadVR(int argc, char** argv);


int main(int argc, char** argv)
{
	ShouldLoadVR(argc, argv);

#pragma region Initialize Window

	WindowContext windowContext;
	windowContext = InitializeWindow();

#pragma endregion 
#pragma region General Initialization

	MeshManager::Allocate();
	MeshManager& meshManager = MeshManager::Get();
	meshManager.InitializeAndLoadMeshes();

	EntityManager::Allocate();
	EntityManager& entityManager = EntityManager::Get();
	entityManager.SpawnDirLight();

	ShaderResources shaderResources = LoadShaders();

	RenderResources renderResources;
	renderResources = InitializeRenderResources();

	TextureResources textureResources;
	textureResources = LoadTextures();

	// Initialize RenderingManager
	RenderingManager renderingManager;
	renderingManager.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, &shaderResources, &renderResources.meshMap, &renderResources.VAOs, &textureResources);

	// Setup Dear ImGui context
	ImguiManager imguiManager;
	imguiManager.InitializeImGui(windowContext);

	Physics::PhysicsManager physicsManager;
	physicsManager.InitializeTestRayCast();

#pragma endregion 

	VRManager vrManager;
	if (vrManager.Initialize(windowContext.window)) {
		std::cout << "VR Manager initialized successfully." << std::endl;
	} else {
		std::cout << "VR Manager is not active." << std::endl;
	}


	bool drawCubes = true;
	bool viewNormals = false;
	Shader currentShader = shaderResources.ourShader;
	float deltaTime = Time::DeltaTime();

	std::vector<std::string> modelNames;
	for (auto& mesh : MeshManager::Get().meshList) {
		modelNames.push_back(mesh->path);
	}

	std::vector<Vertex> currentObject = renderResources.meshMap["Flag.obj"]->vertices;
	unsigned int currentBuffer = renderResources.VAOs[renderResources.meshMap["Flag.obj"]->id];

	glfwSetKeyCallback(windowContext.window, key_callback);

	while (!glfwWindowShouldClose(windowContext.window))
	{
		Input::ActivateInput(windowContext.window);

		deltaTime = Time::DeltaTime();
		physicsManager.SimulatePhysics(deltaTime);

		glClearColor(0.1, 0.1, 0.1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		imguiManager.UpdateImGui(
			drawCubes,
			entityManager,
			modelNames,
			currentObject,
			renderResources.meshMap,
			currentBuffer,
			renderResources.VAOs,
			textureResources.textureNames,
			viewNormals,
			shaderResources.ourShader);

		float near_plane = 1.0f, far_plane = 17.5f;

		//physicsManager.TestRayCast();

		if (drawCubes)
		{
			shaderResources.ourShader.use();

			RenderState renderState;

			Camera::MoveCamera(renderState.view);

			renderState.projection = glm::perspective(glm::radians(Camera::GetFOV()), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);
			renderState.lightPos = entityManager.directionalLight->entityPosition;

			shaderResources.ourShader.setMat4("view", renderState.view);
			shaderResources.ourShader.setMat4("projection", renderState.projection);


			glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(renderState.lightPos,
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 2.0f, 0.0f));

			renderState.lightSpaceMatrix = lightProjection * lightView;

			renderingManager.RenderShadowPass(
				entityManager,
				renderResources,
				textureResources,
				shaderResources.simpleDepthShader,
				renderingManager.GetDepthMap(),
				renderState.lightSpaceMatrix
			);

			for (Entity* entity : entityManager.entities)
			{
				if (entity != nullptr)
				{
					glm::mat4 model;
					model = glm::mat4(1.0f);
					model = glm::translate(model, entity->entityPosition);
					model = glm::rotate(model, glm::radians(entity->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::rotate(model, glm::radians(entity->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::rotate(model, glm::radians(entity->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
					model = glm::scale(model, entity->scale);
					shaderResources.simpleDepthShader.setMat4("model", model);

					currentBuffer = renderResources.VAOs[renderResources.meshMap[entity->model]->id];

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, renderingManager.GetDepthMap());
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, textureResources.textureIDs[entity->textureIndex1]);
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, textureResources.textureIDs[entity->textureIndex2]);
					//glBindTexture(GL_TEXTURE_2D, depthMap);

					glBindVertexArray(renderResources.VAOs[renderResources.meshMap[entity->model]->id]);
					glDrawArrays(GL_TRIANGLES, 0, renderResources.meshMap[entity->model]->vertices.size());
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderContext renderContext = { renderResources.meshMap, renderResources.VAOs, textureResources, shaderResources, renderingManager.GetDepthMap(), viewNormals};

			for (Entity* entity : entityManager.entities)
			{
				if (entity != nullptr)
				{
					glm::mat4 model;
					RenderEntity(
						entity, 
						currentShader, 
						entityManager, 
						renderContext,
						renderState.view,
						renderState.projection,
						renderState.lightSpaceMatrix);
				}
			}
		}
			//debugDepthQuad.use();
			//debugDepthQuad.setInt("depthMap", 0);
			//debugDepthQuad.setFloat("near_plane", near_plane);
			//debugDepthQuad.setFloat("far_plane", far_plane);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, depthMap);
			//renderQuad();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(windowContext.window);
			glfwPollEvents();
	}
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		// Join threads
		//messageThread.join();

		glfwTerminate();
		return 0;
}






void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		Camera::SwitchCamera();
	}
}
void loadResources() {

}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

WindowContext InitializeWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello, World!", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Changed from gladLoadGL to gladLoadGLLoader for GLAD2
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);

    return {window, SCREEN_WIDTH, SCREEN_HEIGHT};
}



ShaderResources LoadShaders() {
    return {
		//Shader ourShader("Shaders/texturelightingshader.vs.txt", "Shaders/texturelightingshader.fs.txt");
//Shader ourShader("Shaders/texturedirectionallightingshader.vs.txt", "Shaders/texturedirectionallightingshader.fs.txt");
//Shader ourShader("Shaders/texturespotlightingshader.vs.txt", "Shaders/texturespotlightingshader.fs.txt");

        Shader("Shaders/lightShader.vs.txt", "Shaders/lightShader.fs.txt"),
		//Shader("Shaders/shadowmapping.vs.txt", "Shaders/shadowmapping.fs.txt"),
        Shader("Shaders/lightingshader.vs.txt", "Shaders/lightingshader.fs.txt"),
        Shader("Shaders/lightsourceshader.vs.txt", "Shaders/lightsourceshader.fs.txt"),
        Shader("Shaders/normalviewshader.vs.txt", "Shaders/normalviewshader.fs.txt"),
        Shader("Shaders/simpledepthshader.vs.txt", "Shaders/simpledepthshader.fs.txt"),
        Shader("Shaders/debugdepthquad.vs.txt", "Shaders/debugdepthquad.fs.txt")
    };
}

TextureResources LoadTextures() {
    TextureResources textures;
    textures.texture1 = Texture::loadTexture("face.png");
    textures.texture2 = Texture::loadTexture("jail.png");
    textures.texture3 = Texture::loadTexture("Pride.png");
    textures.texture4 = Texture::loadTexture("Viking_House.png");
    textures.texture5 = Texture::loadTexture("container.png");
    textures.specularContainerTexture = Texture::loadTexture("container2_specular.png");
    
    textures.textureNames = {"face.png", "jail.png", "Pride.png", "Viking_House.png", "container.png"};
    textures.textureIDs = {textures.texture1, textures.texture2, textures.texture3, textures.texture4, textures.texture5};
    
    return textures;
}
void RenderScene()
{

}

void ShouldLoadVR(int argc, char** argv) {
	// Run in VR if launched with --vr
	    for (int i = 1; i < argc; ++i) {
	        if (std::strcmp(argv[i], "--vr") == 0) {
	#ifdef _WIN32
	            // Set up console close handler for VR mode
	            if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
	                std::cerr << "Warning: Could not set console handler" << std::endl;
	            }
	#endif
	            {
	                //auto program = sample::CreateOpenXrProgram("CrestEngine", sample::CreateCubeGraphics());
	                //program->Run();
	            }
	            return;
	        }
	    }

}

void SetupShaderLighting(Shader& shader, EntityManager& entityManager)
{
	shader.setVec3("dirLight.direction", -entityManager.directionalLight->entityPosition);
	shader.setVec3("dirLight.ambient", entityManager.directionalLight->ambient);
	shader.setVec3("dirLight.diffuse", entityManager.directionalLight->diffuse);
	shader.setVec3("dirLight.specular", entityManager.directionalLight->specular);

	// Setup point lights
	for (int i = 0; i < entityManager.lightSources.size(); i++) {
		shader.setBool("pointLights[" + std::to_string(i) + "].hasSpawned", true);
		shader.setVec3("pointLights[" + std::to_string(i) + "].position", entityManager.lightSources[i]->entityPosition);
		shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", entityManager.lightSources[i]->ambient);
		shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", entityManager.lightSources[i]->diffuse);
		shader.setVec3("pointLights[" + std::to_string(i) + "].specular", entityManager.lightSources[i]->specular);
		shader.setFloat("pointLights[" + std::to_string(i) + "].constant", entityManager.lightSources[i]->constant);
		shader.setFloat("pointLights[" + std::to_string(i) + "].linear", entityManager.lightSources[i]->linear);
		shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", entityManager.lightSources[i]->quadratic);
	}

	// Setup spot light
	shader.setVec3("spotLight.position", Camera::GetCameraPos());
	shader.setVec3("spotLight.direction", Camera::GetCameraFront());
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	shader.setVec3("spotLight.ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09f);
	shader.setFloat("spotLight.quadratic", 0.032f);
}

void RenderEntity(Entity* entity, Shader& currentShader, 
	EntityManager& entityManager, const RenderContext& context,
	const glm::mat4& view, const glm::mat4& projection, 
	const glm::mat4& lightSpaceMatrix)
{
	switch (entity->objectShaderType) {
	case LIGHT_SOURCE_SHADER:
		currentShader = context.shaderResources.lightSourceShader;
		currentShader.use();
		currentShader.setMat4("view", view);
		currentShader.setMat4("projection", projection);
		break;

	case OBJECT_SHADER:
		currentShader = context.shaderResources.ourShader;
		currentShader.use();
		currentShader.setFloat("textureMixer", entity->textureMixer);

		SetupShaderLighting(currentShader, entityManager);

		currentShader.setFloat("material.shininess", 32.0f);
		currentShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		currentShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		currentShader.setVec3("viewPos", Camera::GetCameraPos());
		currentShader.setVec3("light.position", Camera::GetCameraPos());
		currentShader.setVec3("light.direction", Camera::GetCameraFront());
		currentShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
		currentShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
		currentShader.setVec3("lightPos", glm::vec3(0.0f, 3.0f, -20.0f));
		currentShader.setMat4("view", view);
		currentShader.setMat4("projection", projection);
		currentShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		currentShader.setBool("viewSpecular", entity->viewContainerSpecularMap);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, context.depthMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, context.textureResources.textureIDs[entity->textureIndex1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, context.textureResources.textureIDs[entity->textureIndex2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, context.textureResources.specularContainerTexture);
		break;

	case LIGHTING_SHADER:
		currentShader = context.shaderResources.lightingShader;
		currentShader.use();
		currentShader.setMaterial("material", TURQUOISE);
		currentShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
		currentShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		currentShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		currentShader.setFloat("material.shininess", 32.0f);
		currentShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		currentShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		currentShader.setVec3("lightPos", entityManager.directionalLight->entityPosition);
		currentShader.setVec3("viewPos", Camera::GetCameraPos());
		currentShader.setMat4("view", view);
		currentShader.setMat4("projection", projection);
		break;

	default:
		break;
	}

	if (context.viewNormals) {
		currentShader = context.shaderResources.normalViewShader;
		currentShader.use();
		currentShader.setMat4("view", view);
		currentShader.setMat4("projection", projection);
	}

	glBindVertexArray(context.VAOs.at(context.meshMap.at(entity->model)->id));

	entity->position = glm::vec3(entity->entityPosition[0], entity->entityPosition[1], entity->entityPosition[2]);
	entity->rotation = glm::vec3(entity->entityRotation[0], entity->entityRotation[1], entity->entityRotation[2]);
	entity->scale = glm::vec3(entity->entityScale[0], entity->entityScale[1], entity->entityScale[2]);

	glm::mat4 model = entity->CreateTransformMatrix(entity->position, entity->rotation, entity->scale);
	currentShader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)context.meshMap.at(entity->model)->vertices.size());
}

RenderResources InitializeRenderResources() {
    RenderResources resources;
    
    MeshManager::Get().WaitForMeshLoadingComplete();
    
    for (auto& mesh : MeshManager::Get().meshList) {
        resources.meshMap[mesh->path] = mesh;
    }
    
    for (auto& currentMesh : MeshManager::Get().meshList) {
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, currentMesh->vertices.size() * sizeof(Vertex), 
                     currentMesh->vertices.data(), GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // TexCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Normal attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        resources.VAOs[currentMesh->id] = VAO;
        resources.VBOs[currentMesh->id] = VBO;
    }
    
    // Initialize light VAO
    glGenVertexArrays(1, &resources.lightVAO);
    glBindVertexArray(resources.lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, resources.VBOs[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    return resources;
}
