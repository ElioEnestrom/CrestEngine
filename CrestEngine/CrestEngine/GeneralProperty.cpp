#include "GeneralProperty.h"
#include "Entities.h"

GeneralProperty::GeneralProperty() : EntityProperty(PropertyType::GENERALDATA)
{

}

GeneralProperty::~GeneralProperty()
{
}

void GeneralProperty::UpdatePropertyValues(Entity* entity)
{
	name = entity->name;
	position = entity->position;
	rotation = entity->rotation;
	entityPosition[0] = entity->entityPosition[0];
	entityPosition[1] = entity->entityPosition[1];
	entityPosition[2] = entity->entityPosition[2];
	entityRotation[0] = entity->entityRotation[0];
	entityRotation[1] = entity->entityRotation[1];
	entityRotation[2] = entity->entityRotation[2];
	entityScale[0] = entity->entityScale[0];
	entityScale[1] = entity->entityScale[1];
	entityScale[2] = entity->entityScale[2];
}
bool GeneralProperty::WriteTo(std::iostream& file) const
{
    size_t nameLength = name.size();
    file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    file.write(name.c_str(), nameLength);
    file.write(reinterpret_cast<const char*>(&position), sizeof(position));
    file.write(reinterpret_cast<const char*>(&rotation), sizeof(rotation));
    file.write(reinterpret_cast<const char*>(&entityPosition), sizeof(entityPosition));
    file.write(reinterpret_cast<const char*>(&entityRotation), sizeof(entityRotation));
	file.write(reinterpret_cast<const char*>(&entityScale), sizeof(entityScale));

    return file.good();
}


bool GeneralProperty::ReadFrom(std::iostream& file)
{
    size_t nameLength;
    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    name.resize(nameLength);
    file.read(&name[0], nameLength);
    file.read(reinterpret_cast<char*>(&position), sizeof(position));
    file.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));
    file.read(reinterpret_cast<char*>(&entityPosition), sizeof(entityPosition));
    file.read(reinterpret_cast<char*>(&entityRotation), sizeof(entityRotation));
	file.read(reinterpret_cast<char*>(&entityScale), sizeof(entityScale));

    return file.good();
}

void GeneralProperty::UpdateEntity(Entity* entity) const
{
    //If statement to check if name is null
	if (name.empty())
	{
		std::cerr << "Name is empty." << std::endl;
		return;
	}
	entity->name = name;
	entity->position = position;
	entity->rotation = rotation;
	entity->entityPosition[0] = entityPosition[0];
	entity->entityPosition[1] = entityPosition[1];
	entity->entityPosition[2] = entityPosition[2];
	entity->entityRotation[0] = entityRotation[0];
	entity->entityRotation[1] = entityRotation[1];
	entity->entityRotation[2] = entityRotation[2];
	entity->entityScale[0] = entityScale[0];
	entity->entityScale[1] = entityScale[1];
	entity->entityScale[2] = entityScale[2];
}

