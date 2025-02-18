#pragma once
#include <iostream>
#include "EntityProperty.h"

class ModelProperty : public EntityProperty
{
public:
	ModelProperty();
	virtual ~ModelProperty();
	virtual bool WriteTo(std::iostream& file) const override;
	virtual bool ReadFrom(std::iostream& file) override;

private:
	std::string model;
};