#include "Entities.h"

namespace {
    int amountOfEntities = 0;
};
    
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

    

    amountOfEntities++;
    
    entities.push_back(newEntity);

    return newEntity;
}

bool Entity::WriteTo(std::iostream& file) const
{
    return false;
}

bool Entity::ReadFrom(std::iostream& file)
{
    return false;
}


void EntityManager::DeleteEntity(Entity* toDelete)
{
    entities.erase(std::remove(entities.begin(), entities.end(), toDelete), entities.end());
    delete toDelete;
}