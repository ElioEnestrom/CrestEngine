#include "Entities.h"

namespace {
    int amountOfEntities = 0;
};
    
Entity* EntityManager::CreateEntity()
{
    Entity* newEntity = new Entity();
    
    newEntity->name = "Entity " + std::to_string(amountOfEntities);
    newEntity->transform = glm::mat4(1.0f);
    newEntity->position = glm::vec3(0, 0, -3.0f);
    amountOfEntities++;
    
    entities.push_back(newEntity);

    return newEntity;
}

void EntityManager::DeleteEntity(Entity* toDelete)
{
    entities.erase(std::remove(entities.begin(), entities.end(), toDelete), entities.end());
    delete toDelete;
}
