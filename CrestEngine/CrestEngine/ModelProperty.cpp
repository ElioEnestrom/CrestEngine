#include "ModelProperty.h"
#include "Entities.h"

ModelProperty::ModelProperty() : EntityProperty(PropertyType::MODEL)
{
	model = "Flag.obj";
}

ModelProperty::~ModelProperty() {}



bool ModelProperty::WriteTo(std::iostream& file) const
{
    size_t modelLength = model.size();
    file.write(reinterpret_cast<const char*>(&modelLength), sizeof(modelLength));
    file.write(model.c_str(), modelLength);

    return file.good();
}

bool ModelProperty::ReadFrom(std::iostream& file)
{
    size_t modelLength;
    file.read(reinterpret_cast<char*>(&modelLength), sizeof(modelLength));
    model.resize(modelLength);
    file.read(&model[0], modelLength);

    return file.good();
}

void ModelProperty::UpdateEntity(Entity* entity) const 
{
	entity->model = model;
}

void ModelProperty::UpdatePropertyValues(Entity* entity)
{
	model = entity->model;
}
