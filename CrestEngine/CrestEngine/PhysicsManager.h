#pragma once
#include <vector>
#include "Collision.h"
#include "RayCast.h"

const float GravityMultiplier = -9.82f;
const float Restitution = 0.2f;
const float LinearDrag = 0.32f;
const float AngularDrag = 0.000005f;

namespace Physics {
	class PhysicsManager {
	public:
		PhysicsManager();
	
		void SimulatePhysics(const float& deltaTime);
		void UpdateVisuals();
	
		void ApplyVelocity(std::vector<Collider*> colliders, const float& deltaTime);
		void ApplyGravity(std::vector<Collider*> colliders, const float& deltaTime);

		void HandleCollision(std::vector<Collision> colliders);
		void HandleStaticDynamic(std::vector<Collision> collisions);
		void HandleDynamicDynamic(std::vector<Collision> collisions);

	

		std::vector<Collider*> UpdatePhysicsScene();
		std::vector<Collision> CheckIntersections(std::vector<Collider*> colliders);
	
	
		bool RayCast(Ray& aRay, RayHit& aHit);
		bool CheckRayIntersect(const Ray& aRay, Collider* aCollider);
	
	
		std::vector<Collider*> colliders;
		PlaneCollider* main_plane;
	};
}