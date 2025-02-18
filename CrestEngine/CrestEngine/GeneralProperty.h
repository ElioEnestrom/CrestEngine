#pragma once
#include <iostream>
#include "glm/glm.hpp"
#include "EntityProperty.h"

class GeneralProperty : public EntityProperty
{
public:
	GeneralProperty();
	virtual ~GeneralProperty();
	virtual bool WriteTo(std::iostream& file) const override;
	virtual bool ReadFrom(std::iostream& file) override;

private:
	std::string name;
	glm::vec3 position;
	glm::vec3 rotation;
	float entityPosition[3] = { 0.0f, -5.0f, -20.0f };
	float entityRotation[3] = { 0.0f, 0.0f, 0.0f };
};