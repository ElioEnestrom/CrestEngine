#pragma once
#include "glm/glm.hpp"

namespace Physics {
	class Collider;
}

class Ray {
public:
	Ray(glm::vec3 anOrigin, glm::vec3 aDirection) {
		origin = anOrigin;
		direction = aDirection;
	}

	glm::vec3 origin;
	glm::vec3 direction;
};

struct RayHit {
public:
	RayHit() {}
	RayHit(glm::vec3 aPoint, Physics::Collider* aCollider, float aDistance) {
		point = aPoint;
		collider = aCollider;
		distance = aDistance;
	}

	glm::vec3 point;
	Physics::Collider* collider;
	float distance;
};