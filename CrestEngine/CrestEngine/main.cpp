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

#include <thread>
#include <mutex>

#include "Shader.h"
#include "Entities.h"
#include "Camera.h"
#include "Time.h"
#include "Input.h"
#include "Texture.h"
#include "MeshManager.h"
#include <queue>
#include <atomic>
#include <condition_variable>
#include <functional>

// Global variables for threading
std::thread resourceLoaderThread;
std::mutex resourceMutex;
std::condition_variable resourceCondition;
std::atomic<bool> isResourceLoaded(false); // To track the loading status
std::atomic<bool> stopLoading(false);      // To signal the loader thread to exit
Mesh* loadedMesh = nullptr;               // Temporary storage for the loaded mesh

void loadResourcesAsync(const std::string& filePath) {
	Mesh* mesh = nullptr;

	try {
		// Load the mesh data
		mesh = MeshManager::Get().loadOBJ(filePath);
		std::cout << "Loaded " << mesh->vertices.size() / 3 << " vertices from " << filePath << std::endl;
	}
	catch (const std::exception& ex) {
		std::cerr << "Error loading resource: " << ex.what() << std::endl;
	}

	// Safely update the shared resource
	{
		std::lock_guard<std::mutex> lock(resourceMutex);
		loadedMesh = mesh;
		isResourceLoaded = true;
	}

	// Notify the main thread
	resourceCondition.notify_one();
}
void initializeResourceLoader() {
	// Start the resource loading thread
	stopLoading = false;
	resourceLoaderThread = std::thread(loadResourcesAsync, "Flag.obj");
}

void finalizeResourceLoader() {
	// Gracefully stop the thread if it's running
	stopLoading = true;
	if (resourceLoaderThread.joinable()) {
		resourceLoaderThread.join();
	}
}
void updateLoadedResources(unsigned int VBO, unsigned int VAO, unsigned int currentBuffer, std::vector<Vertex> currentObject) {
	// Check if the resource is loaded and update it in the main thread
	if (isResourceLoaded) {
		std::lock_guard<std::mutex> lock(resourceMutex);
		if (loadedMesh) {
			// Bind and upload data to the appropriate VBO/VAO
			glBindBuffer(GL_ARRAY_BUFFER, VBO); // Use your existing VBO1
			glBufferData(GL_ARRAY_BUFFER, loadedMesh->vertices.size() * sizeof(Vertex), loadedMesh->vertices.data(), GL_STATIC_DRAW);

			currentObject = loadedMesh->vertices;
			currentBuffer = VAO; // Assuming VAO1 corresponds to this resource

			std::cout << "Resource loaded and updated in the main thread." << std::endl;

			// Clean up the temporary mesh pointer
			loadedMesh = nullptr;
		}
		isResourceLoaded = false; // Reset the flag
	}
}


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

	initializeResourceLoader();
	Mesh* mesh;
	mesh = MeshManager::Get().loadOBJ("Flag.obj");
	//MeshManager::loadOBJ("Cube.obj");


	std::cout << "Loaded " << mesh->vertices.size() / 3 << " vertices." << std::endl;
	std::cout << "Loaded " << finalVerticesCube.size() / 3 << " vertices." << std::endl;


	unsigned int VBO1, VBO2, VAO1, VAO2, EBO, positionBuffer, textureBuffer, normalBuffer, currentBuffer;
	glGenVertexArrays(1, &VAO1);
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &textureBuffer);
	glGenBuffers(1, &normalBuffer);

	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, loadedMesh->vertices.size() * sizeof(Vertex), loadedMesh->vertices.data(), GL_STATIC_DRAW);
	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, finalVerticesCube.size() * sizeof(Vertex), finalVerticesCube.data(), GL_STATIC_DRAW);


	//std::cout << OBJpositionIndex.size();
	//
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, OBJpositionIndex.size() * sizeof(unsigned int), OBJpositionIndex.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, OBJnormalIndex.size() * sizeof(unsigned int), OBJnormalIndex.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, OBJtextureIndex.size() * sizeof(unsigned int), OBJtextureIndex.data(), GL_STATIC_DRAW);

	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

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
	ImGui_ImplOpenGL3_Init();

	bool drawCubes = true;
	float xLocation = 0;
	float yLocation = 0;
	//glm::mat4 trans = glm::mat4(1.0f);

	EntityManager entityManager;
	Entity* currentlySelected = nullptr;
	float cubePosition[3] = { 0, -5, -10.0f};
	float cubeRotation[3] = { 0, 0, 0};

	glfwSetKeyCallback(window, key_callback);

	std::vector<std::string> modelNames = { "Flag", "Cube"};
	int currentModelIndex = 0;
	currentBuffer = VBO1;
	currentObject = loadedMesh->vertices;
	
	std::vector<std::string> textureNames = { "face.png", "jail.png", "Pride.png"};
	std::vector<unsigned int> textureIDs = { texture1, texture2, texture3 };
	int texture1Index = 0;
	int texture2Index = 1;
	int currentTextureIndex = 0; 
	float textureMixer = 0.35f;


	while (!glfwWindowShouldClose(window))
	{
		Input::ActivateInput(window);
		Time::DeltaTime();

		updateLoadedResources(VBO1, VAO1, currentBuffer, currentObject);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.4, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIDs[texture1Index]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureIDs[texture2Index]);

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
					cubePosition[0] = {entity->position.x};
					cubePosition[1] = {entity->position.y};
					cubePosition[2] = {entity->position.z};
					cubeRotation[0] = { entity->rotation.x };
					cubeRotation[1] = { entity->rotation.y };
					cubeRotation[2] = { entity->rotation.z };
				}
				currentlySelected = entity; 
				//std::cout << currentlySelected->name.c_str();

			}
			}
		ImGui::EndListBox();
		}
		const char* comboPreviewValue = modelNames[currentModelIndex].c_str(); // Preview value
		if (ImGui::BeginCombo("Select Model", comboPreviewValue))
		{
			for (int n = 0; n < modelNames.size(); n++)
			{
				const bool isSelected = (currentModelIndex == n);
				if (ImGui::Selectable(modelNames[n].c_str(), isSelected))
				{
					currentModelIndex = n; // Update the current model index

					if (modelNames[n] == "Flag")
					{
						//currentObject = mesh->vertices;
						//currentBuffer = VAO1;
					}
					else if (modelNames[n] == "Cube")
					{
						//currentObject = finalVerticesCube;
						//currentBuffer = VAO2;
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
	finalizeResourceLoader();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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
