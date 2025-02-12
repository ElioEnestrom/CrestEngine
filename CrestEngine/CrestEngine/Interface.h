#pragma once
#include "Mesh.h"
#include <fstream>


class Interface {
	virtual bool Serialize(std::fstream& stream) = 0;
	virtual bool Deserialize(std::fstream& stream) const= 0;
};