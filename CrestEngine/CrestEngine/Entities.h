#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>

struct Entity {
	std::string name;
	std::string model;
	//glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 rotation;
	float entityPosition[3];
	float entityRotation[3];
	float textureMixer;
	int textureIndex1;
	int textureIndex2;
	//Component component;
	//std::string modelSrc;
	//std::string textureSrc;
};

//class Component
//{
//
//};

 class EntityManager {
 public:
	Entity* CreateEntity();
	
	void DeleteEntity(Entity* toDelete);

	std::vector<Entity*> entities;
 private:
 };