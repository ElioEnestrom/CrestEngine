#pragma once

#include <iostream>
#include "Interface.h"

enum PropertyType
{
	GRAPHIC,
	GENERALDATA,
	MODEL
};

class Entity; 
class GraphicProperty;
class GeneralProperty;
class ModelProperty;

class EntityProperty : public IWritable
{
public:
	EntityProperty(PropertyType type);
	virtual ~EntityProperty();


	virtual bool WriteTo(std::iostream& file) const override;
	virtual bool ReadFrom(std::iostream& file) override;

	virtual void UpdateEntity(Entity* entity) const = 0;
	virtual void UpdatePropertyValues(Entity* entity) = 0;

	PropertyType type;
};