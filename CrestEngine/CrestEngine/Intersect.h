#pragma once
#include "Collision.h"
#include "Raycast.h"

namespace Physics {

	Collision CheckIntersect(Collider* collider1, Collider* collider2);

	Collision SphereSphereIntersect(const SphereCollider& sphere1, const SphereCollider& sphere2);
	Collision SphereBoxIntersect(const SphereCollider& sphere, const BoxCollider& box);
	Collision BoxBoxIntersect(const BoxCollider& box1, const BoxCollider& box2);
	Collision PlaneBoxIntersect(const PlaneCollider& aPlane, const BoxCollider& aBox);
	Collision PlaneSphereIntersect(const PlaneCollider& aPlane, const SphereCollider& aSphere);


	bool RaySphereIntersect(Ray& aRay, SphereCollider& aSphere);
	bool RayBoxIntersect(Ray& aRay, BoxCollider& aBox);
}