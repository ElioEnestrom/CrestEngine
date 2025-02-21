#include "EntityProperty.h"

EntityProperty::EntityProperty(PropertyType type)
{
	this->type = type;
}

EntityProperty::~EntityProperty()
{

}


bool EntityProperty::WriteTo(std::iostream& file) const
{
	return false;
}

bool EntityProperty::ReadFrom(std::iostream& file)
{
	return false;
}
