#pragma once

#include "Interface.h"
#include <glad/glad.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>

struct Entity : public IWritable {
	std::string name;
	std::string model;
	//glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 rotation;
	float entityPosition[3] = {0.0f, -5.0f, -20.0f};
	float entityRotation[3] = {0.0f, 0.0f, 0.0f};
	float textureMixer = 0.0f;
	int textureIndex1 = 0;
	int textureIndex2 = 0;
	//Component component;
	//std::string modelSrc;
	//std::string textureSrc;

	bool WriteTo(std::iostream& file) const override;
	bool ReadFrom(std::iostream& file) override;
};



 class EntityManager {
 public:
	Entity* CreateEntity();
	
	void DeleteEntity(Entity* toDelete);

	std::vector<Entity*> entities;
 private:
 };