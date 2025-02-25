#pragma once
#include "Collider.h"
#include "glm/glm.hpp"

class BoxCollider : public Collider {
	public:
	BoxCollider(const glm::vec3& aCenter, const glm::vec3& someExtents) 
	{
		velocity = glm::vec3(0.0f);
		hasGravity = false;
		center = aCenter;
		extents = someExtents;
	}
	glm::vec3 extents;
};