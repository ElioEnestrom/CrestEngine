#include "ModelProperty.h"

ModelProperty::ModelProperty() : EntityProperty(PropertType::MODEL)
{
	model = "Flag.obj";
}


bool ModelProperty::WriteTo(std::iostream& file) const
{
	file.write(reinterpret_cast<const char*>(&model), sizeof(model));
    return false;
}

bool ModelProperty::ReadFrom(std::iostream& file)
{
	file.read(reinterpret_cast<char*>(&model), sizeof(model));
    return false;
}
