#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider() {

		velocity = glm::vec3(0.0f);
		hasGravity = false;
		radius = 1.0f;
	}
	SphereCollider(float radius) {
		this->radius = radius;
	}
	float radius;
};