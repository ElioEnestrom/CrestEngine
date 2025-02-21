#include "Level.h"


Level::Level() : levelName("Level.binaryThingy", std::ios::in | std::ios::out | std::ios::binary)
{
	// Open the file in both input and output mode
	levelName.open("Level.binaryThingy", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
	if (!levelName.is_open()) {
		std::cerr << "Failed to open level file." << std::endl;
	}
}

//void Level::Allocate()
//{
//	if (instance != nullptr)
//		return;
//	instance = new Level();
//}
//
//Level& Level::Get()
//{
//	return *instance;
//}

void Level::LoadLevel(EntityManager entityManager)
{
	levelName.clear();
	levelName.seekg(0, std::ios::beg);

	for (entities : entityManager.entities)
	{
		entityManager.entities->DeleteEntity(entityManager.entities);
	}
	
	for (int i = 0; i < entityManager.amountOfEntities; i++)
	{
		entityManager.entities[i]->ReadFrom(levelName);
	}
}

void Level::SaveLevel(EntityManager entityManager)
{
	levelName.clear();
	levelName.seekp(0, std::ios::beg);

	for (int i = 0; i < entityManager.amountOfEntities; i++)
	{
		entityManager.entities[i]->UpdateProperties(entityManager.entities[i]);
		entityManager.entities[i]->WriteTo(levelName);
	}
}