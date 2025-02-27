#pragma once

#include "Interface.h"
#include "GeneralProperty.h"
#include "ModelProperty.h"
#include "GraphicProperty.h"
#include "EntityProperty.h"
#include "Collision.h"

#include <glad/glad.h>
#include <string>
#include <vector>
#include <list>
#include <glm/glm.hpp>

struct Entity : public IWritable {
public:
	std::string name;
	std::string model;
	//glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 transform;

	float entityPosition[3] = {0.0f, -5.0f, -20.0f};
	float entityRotation[3] = {0.0f, 0.0f, 0.0f};
	float textureMixer = 0.0f;
	int textureIndex1 = 0;
	int textureIndex2 = 0;

	GeneralProperty generalProperty;
	ModelProperty modelProperty;
	GraphicProperty graphicProperty;
	
	Physics::Collider* GetCollider();

	EntityProperty* AllocateFor(PropertyType type); 
	void AddProperty(EntityProperty* property); // Add method to add properties

	std::vector<EntityProperty*> properties;

	//Component component;
	//std::string modelSrc;
	//std::string textureSrc;

	void UpdateProperties(Entity* property);


	bool WriteTo(std::iostream& file) const override;
	bool ReadFrom(std::iostream& file) override;
private:
	Physics::Collider* myCollider;
};



 class EntityManager {
 public:
	Entity* CreateEntity();
	static void Allocate();
	static EntityManager& Get();
	
	void DeleteEntity(Entity* toDelete);

	void SaveLevel(std::iostream& file);
	void LoadLevel(std::iostream& file);

	std::vector<Entity*> entities;
	int amountOfEntities = 0;
	int amountOfLevels = 0;

 private:
	 static EntityManager* instance;
 };