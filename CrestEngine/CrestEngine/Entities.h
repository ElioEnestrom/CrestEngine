#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>

struct Entity {
	std::string name;
	glm::mat4 transform;
	glm::vec3 position;
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