#pragma once

#include <iostream>
#include "EntityProperty.h"

class Entity;

class GraphicProperty : public EntityProperty
{
public:
	GraphicProperty();
	virtual ~GraphicProperty();
	virtual bool WriteTo(std::iostream& file) const override;
	virtual bool ReadFrom(std::iostream& file) override;

	void UpdateEntity(Entity* entity) const override;
	void UpdatePropertyValues(Entity* entity) override;

private:
	int textureIndex1;
	int textureIndex2;
	float textureMixer;
};