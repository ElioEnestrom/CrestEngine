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

void Level::LoadLevel(EntityManager& entityManager)
{
	levelName.clear();
	levelName.seekg(0, std::ios::beg);

	while (!entityManager.entities.empty())
	{
		Entity* entity = entityManager.entities.back();
		entityManager.DeleteEntity(entity);
	}
	
	entityManager.LoadLevel(levelName);
}

void Level::SaveLevel(EntityManager& entityManager)
{
	levelName.clear();
	levelName.seekp(0, std::ios::beg);

	entityManager.SaveLevel(levelName);
}