#pragma once
#include <iostream>
#include "EntityProperty.h"

class Entity;

class ModelProperty : public EntityProperty
{
public:
	ModelProperty();
	virtual ~ModelProperty();
	virtual bool WriteTo(std::iostream& file) const override;
	virtual bool ReadFrom(std::iostream& file) override;

	void UpdateEntity(Entity* entity) const override;
	void UpdatePropertyValues(Entity* entity) override;
private:
	std::string model;
};