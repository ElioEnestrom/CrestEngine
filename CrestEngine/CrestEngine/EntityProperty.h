#pragma once

#include <iostream>
#include "Interface.h"
#include "GraphicProperty.h"

enum PropertType
{
	GRAPHIC,
	GENERALDATA,
	MODEL
};

class EntityProperty : public IWritable
{
public:
	EntityProperty(PropertType type);
	virtual ~EntityProperty();

	virtual bool WriteTo(std::iostream& file) const override;
	virtual bool ReadFrom(std::iostream& file) override;
};