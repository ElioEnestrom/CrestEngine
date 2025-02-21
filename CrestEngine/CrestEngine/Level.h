#pragma once
#include "Entities.h"
#include <sstream>
#include <fstream>
#include <iostream>

class Level 
{
public:
	Level();

	//static void Allocate();
	//static Level& Get();

	void LoadLevel(EntityManager& entityManager);

	void SaveLevel(EntityManager& entityManager);

	std::fstream levelName;

private:
	//static Level* instance;
};