#include "Entities.h"
#include <sstream>
    
Entity* EntityManager::CreateEntity()
{
    Entity* newEntity = new Entity();
    
    newEntity->name = "Entity " + std::to_string(amountOfEntities);
    newEntity->model = "Flag.obj";
    //newEntity->transform = glm::mat4(1.0f);
    newEntity->position = glm::vec3(0, -5.0f, -10.0f);
    newEntity->rotation = glm::vec3(0, 0, 0);
	newEntity->textureIndex1 = 0;
	newEntity->textureIndex2 = 1;
	newEntity->entityPosition[0] = 0.0f;
	newEntity->entityPosition[1] = -5.0f;
	newEntity->entityPosition[2] = -20.0f;
	newEntity->entityRotation[0] = 0.0f;
	newEntity->entityRotation[1] = 0.0f;
	newEntity->entityRotation[2] = 0.0f;
	newEntity->textureMixer = 0.5f;

    //newEntity->generalProperty.

	newEntity->AddProperty(new GeneralProperty());
    newEntity->AddProperty(new ModelProperty());
    newEntity->AddProperty(new GraphicProperty());

	newEntity->UpdateProperties(newEntity);
    
    amountOfEntities++;
    
    entities.push_back(newEntity);

    return newEntity;
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