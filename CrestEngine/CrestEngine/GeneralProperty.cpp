#include "GeneralProperty.h"

GeneralProperty::GeneralProperty() : EntityProperty(PropertType::GENERALDATA)
{

}

bool GeneralProperty::WriteTo(std::iostream& file) const
{
	file.write(reinterpret_cast<const char*>(&name), sizeof(name));
	file.write(reinterpret_cast<const char*>(&position), sizeof(position));
	file.write(reinterpret_cast<const char*>(&rotation), sizeof(rotation));
	file.write(reinterpret_cast<const char*>(&entityPosition), sizeof(entityPosition));
	file.write(reinterpret_cast<const char*>(&entityRotation), sizeof(entityRotation));

    return false;
}
