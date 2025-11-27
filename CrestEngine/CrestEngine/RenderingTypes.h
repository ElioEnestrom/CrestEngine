#pragma once

#include "Shader.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "Mesh.h"

struct WindowContext {
	GLFWwindow* window;
	unsigned int width;
	unsigned int height;
};

struct ShaderResources {
	Shader ourShader;
	Shader lightingShader;
	Shader lightSourceShader;
	Shader normalViewShader;
	Shader simpleDepthShader;
	Shader debugDepthQuad;
};

struct TextureResources {
	unsigned int texture1;
	unsigned int texture2;
	unsigned int texture3;
	unsigned int texture4;
	unsigned int texture5;
	unsigned int specularContainerTexture;
	std::vector<std::string> textureNames;
	std::vector<unsigned int> textureIDs;
};

struct RenderContext {
	std::unordered_map<std::string, Mesh*> meshMap;
	std::unordered_map<unsigned int, unsigned int> VAOs;
	TextureResources textureResources;
	ShaderResources shaderResources;
	unsigned int depthMap;
	bool viewNormals;
};
