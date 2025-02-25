#pragma once
#include <vector>
#include "Collider.h"
#include "Collision.h"

class SphereCollider;
class BoxCollider;

class PhysicsManager {
public:
	PhysicsManager();

	void SimulatePhysics(const float& deltaTime);

	void ApplyVelocity(std::vector<Collider*> colliders, const float& deltaTime);
	void HandleCollision(std::vector<Collision> colliders);
	std::vector<Collider*> UpdatePhysicsScene();
	std::vector<Collision> CheckCollisions(std::vector<Collider*> colliders);
	void UpdateVisuals();
	void EndStep();

	bool CheckIntersect(Collider* collider1, Collider* collider2);
	bool SphereSphereIntersect(SphereCollider& sphere1, SphereCollider& sphere2);
	bool SphereBoxIntersect(SphereCollider& sphere, BoxCollider& box);
	bool BoxBoxIntersect(BoxCollider& box1, BoxCollider& box2);
};