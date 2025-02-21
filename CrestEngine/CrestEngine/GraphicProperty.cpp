#include "GraphicProperty.h"
#include "Entities.h"

GraphicProperty::GraphicProperty() : EntityProperty(PropertyType::GRAPHIC)
{
	textureIndex1 = 0;
	textureIndex2 = 1;
	textureMixer = 0.5f;
}

GraphicProperty::~GraphicProperty()
{
}

bool GraphicProperty::WriteTo(std::iostream& file) const
{
	file.write(reinterpret_cast<const char*>(&textureIndex1), sizeof(textureIndex1));
	file.write(reinterpret_cast<const char*>(&textureIndex2), sizeof(textureIndex2));
	file.write(reinterpret_cast<const char*>(&textureMixer), sizeof(textureMixer));

	return file.good();
}

bool GraphicProperty::ReadFrom(std::iostream& file)
{
	file.read(reinterpret_cast<char*>(&textureIndex1), sizeof(textureIndex1));
	file.read(reinterpret_cast<char*>(&textureIndex2), sizeof(textureIndex2));
	file.read(reinterpret_cast<char*>(&textureMixer), sizeof(textureMixer));

	return file.good();
}

void GraphicProperty::UpdateEntity(Entity* entity) const
{
	entity->textureIndex1 = textureIndex1;
	entity->textureIndex2 = textureIndex2;
	entity->textureMixer = textureMixer;
}

void GraphicProperty::UpdatePropertyValues(Entity* entity)
{
	textureIndex1 = entity->textureIndex1;
	textureIndex2 = entity->textureIndex2;
	textureMixer = entity->textureMixer;
}
