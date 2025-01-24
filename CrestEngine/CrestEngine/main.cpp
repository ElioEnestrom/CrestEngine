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


#pragma endregion 

	Shader ourShader("firstshader.vs.txt", "firstshader.fs.txt");

	
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

	unsigned int currentBuffer = 0;

	MeshManager::Get().ProcessMessage(new Message(MessageType::Object, "Flag.obj"));
	MeshManager::Get().ProcessMessage(new Message(MessageType::Object, "Cube.obj"));

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

		VAOs[currentMesh->id] = VAO;
		VBOs[currentMesh->id] = VBO; 
		//meshMap[(currentMesh)->path]
	}


	unsigned int texture1 = Texture::loadTexture("face.png");

	unsigned int texture2 = Texture::loadTexture("jail.png");

	unsigned int texture3 = Texture::loadTexture("Pride.png");

	ourShader.use();
	ourShader.setInt("texture2", 1); // or with shader class
	//ourShader.setInt("texture1", 0); // or with shader class
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually

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

	EntityManager entityManager;
	Entity* currentlySelected = nullptr;
	bool drawCubes = true;
	float cubePosition[3] = { 0, -5, -10.0f };
	float cubeRotation[3] = { 0, 0, 0 };

	std::vector<std::string> modelNames;
	for (auto& mesh : MeshManager::Get().meshList) {
		modelNames.push_back(mesh->path);
	}

	int currentModelIndex = 0; 
	
	currentBuffer = VAOs[meshMap["Flag.obj"]->id];
	currentObject = meshMap["Flag.obj"]->vertices;
	
	std::vector<std::string> textureNames = { "face.png", "jail.png", "Pride.png"};
	std::vector<unsigned int> textureIDs = { texture1, texture2, texture3 };
	int texture1Index = 0;
	int texture2Index = 1;
	int currentTextureIndex = 0; 
	float textureMixer = 0.35f;

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
		Time::DeltaTime();


		glClearColor(0.4, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIDs[texture1Index]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureIDs[texture2Index]);

		ImguiManager imguiManager;
		imguiManager.UpdateImGui(
			drawCubes, 
			cubePosition, 
			cubeRotation, 
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
			textureMixer);

		//MessageQueue::QueueMessage(new Message(MessageType::String, "Hello, World!"));


		if (drawCubes)
		{
			ourShader.use();
			ourShader.setFloat("textureMixer", textureMixer);

			glm::mat4 view;

			Camera::MoveCamera(view);
			

			glm::mat4 projection = glm::perspective(glm::radians(Camera::GetFOV()), 800.0f / 600.0f, 0.1f, 100.0f);
			ourShader.setMat4("view", view);
			ourShader.setMat4("projection", projection);

			glBindVertexArray(currentBuffer);

			// Render all entities
			for (Entity* entity : entityManager.entities)
			{
				if (entity == currentlySelected)
				{
					entity->position = glm::vec3(cubePosition[0], cubePosition[1], cubePosition[2]);
					glm::mat4 model = glm::translate(glm::mat4(1.0f), entity->position);
					entity->rotation = glm::vec3(cubeRotation[0], cubeRotation[1], cubeRotation[2]); 
					glm::vec3 rotationAxis = entity->rotation;
					if (glm::length(rotationAxis) > 0.0f) {
						model = glm::rotate(model, glm::radians(rotationAxis.x), glm::vec3(1.0f, 0, 0));
						model = glm::rotate(model, glm::radians(rotationAxis.y), glm::vec3(0, 1.0f, 0));
						model = glm::rotate(model, glm::radians(rotationAxis.z), glm::vec3(0, 0, 1.f));
					}
					ourShader.setMat4("model", model);

					//glDrawElements(GL_TRIANGLES, OBJpositionIndex.size(), GL_UNSIGNED_INT, 0);
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
