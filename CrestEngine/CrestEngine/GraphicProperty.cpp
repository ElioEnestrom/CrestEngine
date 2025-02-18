#include "GraphicProperty.h"

GraphicProperty::GraphicProperty() : EntityProperty(PropertType::GRAPHIC)
{
	textureIndex1 = 0;
	textureIndex2 = 1;
	textureMixer = 0.5f;
}

bool GraphicProperty::WriteTo(std::iostream& file) const
{
	file.write(reinterpret_cast<const char*>(&textureIndex1), sizeof(textureIndex1));
	file.write(reinterpret_cast<const char*>(&textureIndex2), sizeof(textureIndex2));
	file.write(reinterpret_cast<const char*>(&textureMixer), sizeof(textureMixer));

	return false;
}

bool GraphicProperty::ReadFrom(std::iostream& file)
{
	file.read(reinterpret_cast<char*>(&textureIndex1), sizeof(textureIndex1));
	file.read(reinterpret_cast<char*>(&textureIndex2), sizeof(textureIndex2));
	file.read(reinterpret_cast<char*>(&textureMixer), sizeof(textureMixer));

	return false;
}
