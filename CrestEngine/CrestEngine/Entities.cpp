#include "Entities.h"
#include "Collision.h"
#include "Math.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include <sstream>
    
EntityManager* EntityManager::instance = nullptr;

Entity* EntityManager::CreateEntity()
{
    Entity* newEntity = new Entity();
    
    newEntity->name = "Entity " + std::to_string(amountOfEntities);
    newEntity->model = "MySphere.obj";
    //newEntity->transform = glm::mat4(1.0f);
    newEntity->position = glm::vec3(0, 0.0f, -10.0f);
    newEntity->rotation = glm::vec3(0, 0, 0);
    
    newEntity->entityPosition = glm::vec3(0.0f, 0.0f, -20.0f);
    newEntity->entityRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    newEntity->entityScale= glm::vec3(1.0f, 1.0f, 1.0f);
	newEntity->textureMixer = 0.5f;
	newEntity->textureIndex1 = 0;
	newEntity->textureIndex2 = 1; 

    //newEntity->generalProperty.

	newEntity->AddProperty(new GeneralProperty());
    newEntity->AddProperty(new ModelProperty());
    newEntity->AddProperty(new GraphicProperty());

	newEntity->UpdateProperties(newEntity);
    
    amountOfEntities++;

	newEntity->CreateTransformMatrix(newEntity->entityPosition, newEntity->entityRotation, newEntity->entityScale);
    
    entities.push_back(newEntity);

	newEntity->CreateSphereCollider(1.0f);


	//newEntity->myCollider = new Physics::SphereCollider(newEntity->position, 1.0f);
    return newEntity;
}

void EntityManager::Allocate()
{
	if (instance != nullptr)
		return;
	instance = new EntityManager();
}

void Entity::SetTransform(const glm::mat4& aTransform)
{
    glm::vec3 translation, rotation, scale;
    Math::DecomposeTransform(aTransform, translation, rotation, scale);

	entityPosition = translation;
    entityRotation = rotation;
    //Scale = scale;
}
glm::mat4 Entity::GetTransform()
{
    glm::mat4 rotation = glm::toMat4(glm::quat(entityRotation));

    return glm::translate(glm::mat4(1.0f), entityPosition)
        * rotation
        * glm::scale(glm::mat4(1.0f), entityScale);
}
glm::mat4 Entity::CreateTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

    // Apply translation
    transform = glm::translate(transform, position);

    // Apply rotation
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    // Apply scale
    transform = glm::scale(transform, scale);

    return transform;
}
EntityManager& EntityManager::Get()
{
	return *instance;
}

Physics::Collider* Entity::GetCollider()
{
    return myCollider;
}

EntityProperty* Entity::AllocateFor(PropertyType type)
{
	switch (type)
	{
        case PropertyType::GRAPHIC: return new GraphicProperty();
        case PropertyType::GENERALDATA: return new GeneralProperty();
        case PropertyType::MODEL: return new ModelProperty();
        default:
        	return nullptr;
	}
}

void Entity::AddProperty(EntityProperty* property)
{
    properties.push_back(property);
}

void Entity::CreateSphereCollider(const float& aRadius)
{
	myCollider = new Physics::SphereCollider(position, aRadius);
}

void Entity::CreateBoxCollider(const glm::vec3& someExtents)
{
	myCollider = new Physics::BoxCollider(position, someExtents);
}


void Entity::UpdateProperties(Entity* property)
{
	generalProperty.UpdatePropertyValues(property);
	modelProperty.UpdatePropertyValues(property);
	graphicProperty.UpdatePropertyValues(property);
}

bool Entity::WriteTo(std::iostream& file) const
{
    if (!generalProperty.WriteTo(file)) {
        std::cerr << "Failed to write GeneralProperty." << std::endl;
        return false;
    }

    //EntityProperty* genrealPropertyStuff = AllocateFor(static_cast<PropertyType>(generalProperty.type));

    if (!modelProperty.WriteTo(file)) {
        std::cerr << "Failed to write ModelProperty." << std::endl;
        return false;
    }

    if (!graphicProperty.WriteTo(file)) {
        std::cerr << "Failed to write GraphicProperty." << std::endl;
        return false;
    }



    //int numCustomProperties = properties.size();
    //std::cerr << "Writing number of custom properties: " << numCustomProperties << std::endl;
    //file.write(reinterpret_cast<const char*>(&numCustomProperties), sizeof(numCustomProperties));
    //if (!file) {
    //    std::cerr << "Failed to write number of custom properties." << std::endl;
    //    return false;
    //}

    for (EntityProperty* property : properties)
    {
        int propertyType = property->type;
        std::cerr << "Writing property type: " << propertyType << std::endl;
        file.write(reinterpret_cast<const char*>(&propertyType), sizeof(propertyType));
        if (!file) {
            std::cerr << "Failed to write property type." << std::endl;
            return false;
        }

        std::cerr << "Writing property data..." << std::endl;
        if (!property->WriteTo(file)) {
            std::cerr << "Failed to write property data." << std::endl;
            return false;
        }
    }
    return true;
}

bool Entity::ReadFrom(std::iostream& file)
{
    generalProperty.ReadFrom(file);
	generalProperty.UpdateEntity(this);
    std::cerr << "Finished reading GeneralProperty." << std::endl;

    modelProperty.ReadFrom(file);
	modelProperty.UpdateEntity(this);
    std::cerr << "Finished reading ModelProperty." << std::endl;

    graphicProperty.ReadFrom(file);
	graphicProperty.UpdateEntity(this);
    std::cerr << "Finished reading GraphicProperty." << std::endl;

    //int numCustomProperties;
    //file.read(reinterpret_cast<char*>(&numCustomProperties), sizeof(numCustomProperties)); 
    //if (numCustomProperties > 100) {
    //    std::cerr << "Trying to load too many properties." << std::endl;
    //    return false;
    //}
    //std::cerr << "Number of custom properties to read: " << numCustomProperties << std::endl;

	//numCustomProperties = properties.size();

    for (int i = 0; i < properties.size(); i++)
    {
        int propertyType;
        file.read(reinterpret_cast<char*>(&propertyType), sizeof(propertyType));
        std::cerr << "Reading property type: " << propertyType << std::endl;

        EntityProperty* newProperty = AllocateFor(static_cast<PropertyType>(propertyType));
        if (newProperty)
        {
            newProperty->ReadFrom(file);
			newProperty->UpdateEntity(this);
            std::cerr << "Finished reading property of type: " << propertyType << std::endl;
        }
        else
        {
            std::cerr << "Failed to allocate property of type: " << propertyType << std::endl;
        }
    }
    return true;
}


void EntityManager::DeleteEntity(Entity* toDelete)
{
    entities.erase(std::remove(entities.begin(), entities.end(), toDelete), entities.end());
	if (amountOfEntities > 0)
        amountOfEntities--;
    delete toDelete;
}

void EntityManager::SaveLevel(std::iostream& file)
{
	int numEntities = amountOfEntities;
	file.write(reinterpret_cast<const char*>(&numEntities), sizeof(numEntities));
	if (!file) {
		std::cerr << "Failed to write number of entities." << std::endl;
		return;
	}
	for (Entity* entity : entities)
	{
		entity->UpdateProperties(entity);
		entity->WriteTo(file);
	}
}

void EntityManager::LoadLevel(std::iostream& file)
{
	int numEntities;
	file.read(reinterpret_cast<char*>(&numEntities), sizeof(numEntities));
	if (!file) {
		std::cerr << "Failed to read number of entities." << std::endl;
		return;
	}
	for (int i = 0; i < numEntities; i++)
	{
		Entity* entity = CreateEntity();
		entity->ReadFrom(file);
	}
}