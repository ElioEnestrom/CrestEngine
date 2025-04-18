#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>

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



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() 
{

	#pragma region Initialization
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Hello, World!", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	glfwSwapInterval(1);

#pragma endregion 

	//Shader ourShader("Shaders/texturelightingshader.vs.txt", "Shaders/texturelightingshader.fs.txt");
	//Shader ourShader("Shaders/texturedirectionallightingshader.vs.txt", "Shaders/texturedirectionallightingshader.fs.txt");
	//Shader ourShader("Shaders/texturespotlightingshader.vs.txt", "Shaders/texturespotlightingshader.fs.txt");
	Shader ourShader("Shaders/lightShader.vs.txt", "Shaders/lightShader.fs.txt");
	Shader lightingShader("Shaders/lightingshader.vs.txt", "Shaders/lightingshader.fs.txt");
	Shader lightSourceShader("Shaders/lightsourceshader.vs.txt", "Shaders/lightsourceshader.fs.txt");
	Shader normalViewShader("Shaders/normalviewshader.vs.txt", "Shaders/normalviewshader.fs.txt");
	
	std::vector<float> OBJvertices;
	std::vector<float> OBJnormals;
	std::vector<float> OBJtexCoords;
	std::vector<unsigned int> OBJpositionIndex;
	std::vector<unsigned int> OBJtextureIndex;
	std::vector<unsigned int> OBJnormalIndex;
	std::vector<unsigned int> OBJvertexIndex;
	std::vector<Vertex> finalVerticesCube;
	std::vector<Vertex> finalVerticesFlag;
	std::vector<Vertex> currentObject;

	MeshManager::Allocate();
	EntityManager::Allocate();

    for (auto& message : MeshManager::Get().objMessages) {
        MessageQueue::Get().QueueMessage(&message);
    }

	unsigned int currentBuffer = 0;

	std::unordered_map<std::string, Mesh*> meshMap;
	std::unordered_map<unsigned int, unsigned int> VBOs;
	std::unordered_map<unsigned int, unsigned int> VAOs;

	for (auto& mesh : MeshManager::Get().meshList)
	{
		meshMap[mesh->path] = mesh;
	}
	
	for (auto& currentMesh : MeshManager::Get().meshList)
	{
	    unsigned int VAO;
	    unsigned int VBO;
	    glGenVertexArrays(1, &VAO);
	    glGenBuffers(1, &VBO);
	    glBindVertexArray(VAO);
	    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	    glBufferData(GL_ARRAY_BUFFER, currentMesh->vertices.size() * sizeof(Vertex), currentMesh->vertices.data(), GL_STATIC_DRAW);
	
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	    glEnableVertexAttribArray(0);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	    glEnableVertexAttribArray(1);
	    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
	    glEnableVertexAttribArray(2);

		//void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		//if (data) {
		//	Vertex* vertices = static_cast<Vertex*>(data);
		//	for (size_t i = 0; i < currentMesh->vertices.size(); ++i) {
		//		std::cout << "Normal from VBO: ("
		//			<< vertices[i].normal.x << ", "
		//			<< vertices[i].normal.y << ", "
		//			<< vertices[i].normal.z << ")" << std::endl;
		//	}
		//	glUnmapBuffer(GL_ARRAY_BUFFER);
		//}
	    VAOs[currentMesh->id] = VAO;
	    VBOs[currentMesh->id] = VBO; 
	}
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
	// set the vertex attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int texture1 = Texture::loadTexture("face.png");

	unsigned int texture2 = Texture::loadTexture("jail.png");

	unsigned int texture3 = Texture::loadTexture("Pride.png");

	unsigned int texture4 = Texture::loadTexture("Viking_House.png");

	unsigned int texture5 = Texture::loadTexture("container.png");

	unsigned int specularContainerTexture = Texture::loadTexture("container2_specular.png");

	ourShader.use();


	ourShader.setInt("material.specular", 2);
	ourShader.setInt("texture2", 1); // or with shader class
	//ourShader.setInt("texture1", 0); // or with shader class
	glUniform1i(glGetUniformLocation(ourShader.ID, "<texture1>"), 0); // set it manually

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init("#version 330");

	float xLocation = 0;
	float yLocation = 0;
	//glm::mat4 trans = glm::mat4(1.0f);

	EntityManager& entityManager = EntityManager::Get();
	Entity* currentlySelected = nullptr;
	bool drawCubes = true;
	//float cubePosition[3] = { 0, -5, -10.0f };
	//float cubeRotation[3] = { 0, 0, 0 };

	std::vector<std::string> modelNames;
	for (auto& mesh : MeshManager::Get().meshList) {
		modelNames.push_back(mesh->path);
	}

	int currentModelIndex = 0; 
	
	currentBuffer = VAOs[meshMap["Flag.obj"]->id];
	currentObject = meshMap["Flag.obj"]->vertices;
	
	std::vector<std::string> textureNames = { "face.png", "jail.png", "Pride.png", "Viking_House.png", "container.png"};
	std::vector<unsigned int> textureIDs = { texture1, texture2, texture3, texture4, texture5 };
	int texture1Index = 0;
	int texture2Index = 1;
	int currentTextureIndex = 0; 
	float textureMixer = 0.35f;
	Physics::PhysicsManager physicsManager;

	bool viewNormals = false;

	// Define the origin and direction of the ray
	glm::vec3 rayOrigin(0.0f, 0.0f, -20.0f);
	glm::vec3 rayDirection(0.0f, 1.0f, 0.0f);

	// Create a Ray object
	Ray ray(rayOrigin, rayDirection);

	// Create a RayHit object to store the result
	RayHit hit;

	Shader currentShader = ourShader;

	entityManager.SpawnDirLight();

	glfwSetKeyCallback(window, key_callback);
	//std::thread messageThread([] {
	//	while (true)
	//	{
	//		std::cout << "Hello from thread!" << std::endl;
	//		std::this_thread::sleep_for(std::chrono::seconds(1));
	//	}
	//});
	while (!glfwWindowShouldClose(window))
	{
	    Input::ActivateInput(window);
		float deltaTime = Time::DeltaTime();
		//std::cout << Time::DeltaTime() << std::endl;
		physicsManager.SimulatePhysics(deltaTime);

	    glClearColor(0.1, 0.1, 0.1, 1);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	    ImguiManager imguiManager;
	    imguiManager.UpdateImGui(
	        drawCubes, 
	        entityManager, 
	        currentlySelected, 
	        modelNames, 
	        currentModelIndex, 
	        currentObject, 
	        meshMap, 
	        currentBuffer, 
	        VAOs, 
	        textureNames, 
	        texture1Index, 
	        texture2Index, 
	        textureMixer,
			viewNormals,
			ourShader); 
		
		bool hitDetected = physicsManager.RayCast(ray);

		// Check if the ray hit any collider
		if (hitDetected) {
			//std::cout << "Hit collider name: " << ray.hit.collider->GetTypeName() << std::endl;
			//std::cout << "Hit distance: " << ray.hit.distance << std::endl;
		}
	
	    if (drawCubes)
	    {
	        ourShader.use();
	
	        glm::mat4 view;
	        Camera::MoveCamera(view);
	
	        glm::mat4 projection = glm::perspective(glm::radians(Camera::GetFOV()), 800.0f / 600.0f, 0.1f, 100.0f);
	        ourShader.setMat4("view", view);
	        ourShader.setMat4("projection", projection);
	
	        for (Entity* entity : entityManager.entities)
	        {
	            if (entity != nullptr)
	            {
					glm::mat4 model;
                    switch (entity->objectShaderType) {
                        case LIGHT_SOURCE_SHADER:
                            currentShader = lightSourceShader;
                            currentShader.use();
                            currentShader.setMat4("view", view);
                            currentShader.setMat4("projection", projection);
                            break;
                        case OBJECT_SHADER:
                            currentShader = ourShader;
							currentShader.use();
							currentShader.setFloat("textureMixer", entity->textureMixer);
							
							currentShader.setVec3("dirLight.direction", -entityManager.directionalLight->entityPosition);
							currentShader.setVec3("dirLight.ambient", entityManager.directionalLight->ambient);
							currentShader.setVec3("dirLight.diffuse", entityManager.directionalLight->diffuse);
							currentShader.setVec3("dirLight.specular", entityManager.directionalLight->specular);


							// point light 1
							for (int i = 0; i < entityManager.lightSources.size(); i++)
							{
								currentShader.setBool("pointLights[" + std::to_string(i) + "].hasSpawned", true);
								currentShader.setVec3("pointLights[" + std::to_string(i) + "].position", entityManager.lightSources[i]->entityPosition);
								currentShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", entityManager.lightSources[i]->ambient);
								currentShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", entityManager.lightSources[i]->diffuse);
								currentShader.setVec3("pointLights[" + std::to_string(i) + "].specular", entityManager.lightSources[i]->specular);
								currentShader.setFloat("pointLights[" + std::to_string(i) + "].constant", entityManager.lightSources[i]->constant);
								currentShader.setFloat("pointLights[" + std::to_string(i) + "].linear", entityManager.lightSources[i]->linear);
								currentShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", entityManager.lightSources[i]->quadratic);

							}

							currentShader.setVec3("spotLight.position", Camera::GetCameraPos());
							currentShader.setVec3("spotLight.direction", Camera::GetCameraFront());
							currentShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
							currentShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
							currentShader.setVec3("spotLight.ambient", 0.05f, 0.05f, 0.05f);
							currentShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
							currentShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
							currentShader.setFloat("spotLight.constant", 1.0f);
							currentShader.setFloat("spotLight.linear", 0.09f);
							currentShader.setFloat("spotLight.quadratic", 0.032f);
							
							currentShader.setFloat("material.shininess", 32.0f);

							currentShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
							currentShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
							currentShader.setVec3("lightPos", glm::vec3(0, 1.0f, 0.0f));
							currentShader.setVec3("viewPos", Camera::GetCameraPos());


							currentShader.setVec3("light.position", Camera::GetCameraPos());
							currentShader.setVec3("light.direction", Camera::GetCameraFront());
							currentShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
							currentShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

							if (entity->viewContainerSpecularMap) 
							{
								currentShader.setBool("viewSpecular", true);
							}
							else
							{
								currentShader.setBool("viewSpecular", false);
							}

                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, textureIDs[entity->textureIndex1]);
                            glActiveTexture(GL_TEXTURE1);
                            glBindTexture(GL_TEXTURE_2D, textureIDs[entity->textureIndex2]);
							glActiveTexture(GL_TEXTURE2);
							glBindTexture(GL_TEXTURE_2D, specularContainerTexture);
                            break;
                        case LIGHTING_SHADER:
                            currentShader = lightingShader;
                            currentShader.use(); 
							currentShader.setMaterial("material", TURQUOISE);
							currentShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
							currentShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f); // darken diffuse light a bit
							currentShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
							currentShader.setFloat("material.shininess", 32.0f);
                            currentShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
                            currentShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
							currentShader.setVec3("lightPos", glm::vec3(0, 3.0f, -20.0f));
							currentShader.setVec3("viewPos", Camera::GetCameraPos());
                            currentShader.setMat4("view", view);
                            currentShader.setMat4("projection", projection);
                            break;
                        default:
                            // Handle unexpected shader type if necessary
                            break;
                    }
					if (viewNormals) {
						currentShader = normalViewShader;
						currentShader.use();
						currentShader.setMat4("view", view);
						currentShader.setMat4("projection", projection);
					}

	                currentObject = meshMap[entity->model]->vertices;
	                currentBuffer = VAOs[meshMap[entity->model]->id];
	

	                glBindVertexArray(currentBuffer);
	
	                entity->position = glm::vec3(entity->entityPosition[0], entity->entityPosition[1], entity->entityPosition[2]);
	                entity->rotation = glm::vec3(entity->entityRotation[0], entity->entityRotation[1], entity->entityRotation[2]);
					entity->scale = glm::vec3(entity->entityScale[0], entity->entityScale[1], entity->entityScale[2]);
					
					model = entity->CreateTransformMatrix(entity->position, entity->rotation, entity->scale);
					//glm::mat4 model = glm::translate(glm::mat4(1.0f), entity->position);
	                //glm::vec3 rotationAxis = entity->rotation;
	                //if (glm::length(rotationAxis) > 0.0f) {
	                //    model = glm::rotate(model, glm::radians(rotationAxis.x), glm::vec3(1.0f, 0, 0));
	                //    model = glm::rotate(model, glm::radians(rotationAxis.y), glm::vec3(0, 1.0f, 0));
	                //    model = glm::rotate(model, glm::radians(rotationAxis.z), glm::vec3(0, 0, 1.f));
	                //}
	                currentShader.setMat4("model", model);
	
	                // Bind textures for the current entity
	
	                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)currentObject.size());
	            }
	        }
		}
	
	    ImGui::Render();
	    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	    glfwSwapBuffers(window);
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
