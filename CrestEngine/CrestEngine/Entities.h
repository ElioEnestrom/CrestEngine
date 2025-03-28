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

	glm::vec3 entityPosition;
	glm::vec3 entityRotation;
	glm::vec3 entityScale;
	float textureMixer = 0.0f;
	int textureIndex1 = 0;
	int textureIndex2 = 0;

	GeneralProperty generalProperty;
	ModelProperty modelProperty;
	GraphicProperty graphicProperty;
	
	Physics::Collider* GetCollider();

	EntityProperty* AllocateFor(PropertyType type); 
	void AddProperty(EntityProperty* property); // Add method to add properties
	void CreateSphereCollider(const float& aRadius);
	void CreateBoxCollider(const glm::vec3& someExtents);
	void SetTransform(const glm::mat4& aTransform); 
	glm::mat4 CreateTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	glm::mat4 GetTransform();

	std::vector<EntityProperty*> properties;

	//Component component;
	//std::string modelSrc;
	//std::string textureSrc;

	void UpdateProperties(Entity* property);


	bool WriteTo(std::iostream& file) const override;
	bool ReadFrom(std::iostream& file) override; 

	Physics::Collider* myCollider = nullptr;
private:
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