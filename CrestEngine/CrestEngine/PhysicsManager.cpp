#include "PhysicsManager.h"
#include "Intersect.h"
#include "Collision.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Entities.h"

namespace Physics {
	PhysicsManager::PhysicsManager()
	{
		main_plane = new PlaneCollider(glm::vec3(0, 1, 0), 1);
		main_plane->position = glm::vec3(0, 0, 0);
	}
	void PhysicsManager::SimulatePhysics(const float& deltaTime)
	{
		float limitDt = glm::min(deltaTime, 0.02f);

		for (Collider* collider : colliders) {
			std::cout << "Collider: " << collider->position.x << " " << collider->position.y << " " << collider->position.z << std::endl;
			collider->ComputeInertia();
		}

		colliders = UpdatePhysicsScene();
		std::vector<Collision> collisions = CheckIntersections(colliders);

		ApplyGravity(colliders, limitDt);

		HandleCollision(collisions);

		ApplyVelocity(colliders, limitDt);

		UpdateVisuals();
	}
	
	void PhysicsManager::ApplyVelocity(std::vector<Collider*> colliders, const float& deltaTime)
	{
		for (Collider* c : colliders)
		{
			if (!c->isKinematic)
			{
				c->position += c->velocity * deltaTime;
				c->transform[3] = glm::vec4(c->position, 1.0f);

				float maxAngularVelocity = 3;
				if (glm::length(c->angularVelocity) > maxAngularVelocity)
				{
					c->angularVelocity = glm::normalize(c->angularVelocity) * maxAngularVelocity;
				}

				if (glm::length(c->angularVelocity) > 0.0001f)
				{
					glm::vec3 angularVelocityNorm = glm::normalize(c->angularVelocity);
					glm::quat angularRotation = glm::angleAxis(glm::length(c->angularVelocity) * deltaTime, angularVelocityNorm);
					glm::mat3 rotationDelta = glm::mat3_cast(angularRotation);
					c->transform = glm::mat4(rotationDelta) * c->transform;
				}

				if (c->mass > 0)
				{
					c->velocity *= glm::pow(1.0f - LinearDrag, deltaTime);
				}

				if (c->mass > 0)
				{
					c->angularVelocity *= glm::exp(-AngularDrag * deltaTime);
				}

				// inertia tensor in world space
				glm::mat3 rotationMatrix = glm::mat3(c->transform);
				glm::mat3 inertiaTensorInWorldSpace = rotationMatrix * c->momentOfInertia * glm::transpose(rotationMatrix);
				c->inverseMomentOfInertia = glm::inverse(inertiaTensorInWorldSpace);
			}
			else
			{
				c->velocity = glm::vec3(0, 0, 0);
				c->angularVelocity = glm::vec3(0, 0, 0);
			}
		}
	}
	
	void PhysicsManager::HandleCollision(std::vector<Collision> collisions)
	{
		std::vector<Collision> dynamicDynamicCollisions;
		std::vector<Collision> staticDynamicCollisions;

		for (Collision c : collisions)
		{
			bool A_isDynamic = !c.collider1->isKinematic;
			bool B_isDynamic = !c.collider2->isKinematic;

			if (A_isDynamic && B_isDynamic)
			{
				dynamicDynamicCollisions.push_back(c);
			}
			else
			{
				staticDynamicCollisions.push_back(c);
			}
		}

		HandleDynamicDynamic(dynamicDynamicCollisions);
		HandleStaticDynamic(staticDynamicCollisions);
	}

	void PhysicsManager::HandleStaticDynamic(std::vector<Collision> collisions)
	{
		const float SlidingFriction = 0.5f;

		for (Collision c : collisions)
		{
			Collider* A = c.collider1;
			Collider* B = c.collider2;

			bool A_isDynamic = !A->isKinematic;
			bool B_isDynamic = !B->isKinematic;

			if (!A_isDynamic && !B_isDynamic) continue;

			Collider* dynamicCollider = A_isDynamic ? A : B;
			Collider* staticCollider = A_isDynamic ? B : A;

			glm::vec3 n = glm::normalize(c.normal);
			glm::vec3 r = c.point - dynamicCollider->position;

			glm::vec3 v = dynamicCollider->velocity + glm::cross(dynamicCollider->angularVelocity, r);
			float vRelDotN = glm::dot(v, n);

			if (vRelDotN > 0) continue;

			float invMass = (dynamicCollider->mass > 0) ? 1.0f / dynamicCollider->mass : 0;
			glm::vec3 r_cross_n = glm::cross(r, n);
			float angularEffect = glm::dot(r_cross_n, dynamicCollider->inverseMomentOfInertia * r_cross_n);

			float impulseMagnitude = -(1 + Restitution) * vRelDotN / (invMass + angularEffect);
			glm::vec3 impulse = impulseMagnitude * n;

			// Apply impulse to linear velocity
			dynamicCollider->velocity += impulse * invMass;

			// angular velocity (considering moment of inertia)
			dynamicCollider->angularVelocity += dynamicCollider->inverseMomentOfInertia * glm::cross(r, impulse);

			// sliding friction
			glm::vec3 tangentVelocity = v - (n * glm::dot(v, n));
			if (glm::length(tangentVelocity) > 0.0001f)
			{
				glm::vec3 frictionDirection = -glm::normalize(tangentVelocity);
				glm::vec3 frictionImpulse = frictionDirection * SlidingFriction * glm::length(tangentVelocity);

				dynamicCollider->velocity += frictionImpulse * invMass;
				dynamicCollider->angularVelocity += dynamicCollider->inverseMomentOfInertia * glm::cross(r, frictionImpulse);
			}
		}
	}

	void PhysicsManager::HandleDynamicDynamic(std::vector<Collision> collisions)
	{
		for (Collision c : collisions)
		{
			glm::vec3 normal = c.normal;

			glm::vec3 relativeVelocity = c.collider2->velocity - c.collider1->velocity;
			float velocityAlongNormal = glm::dot(relativeVelocity, normal);

			if (velocityAlongNormal > 0) continue;

			float impulse = (1 + Restitution) * velocityAlongNormal;

			glm::vec3 impulseVector = impulse * normal;

			c.collider1->velocity += impulseVector;
			c.collider2->velocity -= impulseVector;

			glm::vec3 r1 = c.point - c.collider1->position;
			glm::vec3 r2 = c.point - c.collider2->position;

			glm::vec3 torque1 = glm::cross(r1, impulseVector);
			glm::vec3 torque2 = glm::cross(r2, impulseVector);

			// Rotate! sort of....
			c.collider1->angularVelocity += c.collider1->inverseMomentOfInertia * torque1;
			c.collider2->angularVelocity -= c.collider2->inverseMomentOfInertia * torque2;
		}
	}
	
	std::vector<Collider*> PhysicsManager::UpdatePhysicsScene()
	{

		std::vector<Collider*> cols;

		cols.push_back(main_plane);


		for (Entity* c : EntityManager::Get().entities)
		{
			Collider* col = c->GetCollider();
			if (col != nullptr)
			{
				glm::mat4 trans = c->transform;

				col->transform = trans;
				col->position = glm::vec3(trans[3]);

				cols.push_back(col);
			}
		}

		return cols;
	}
	
	void PhysicsManager::UpdateVisuals()
	{
		for (Entity* c : EntityManager::Get().entities)
		{
			Collider* col = c->GetCollider();
			if (col != nullptr)
			{
				c->transform = col->transform;
			}
		}
	}

	void PhysicsManager::ApplyGravity(std::vector<Collider*> colliders, const float& deltaTime)
	{
		for (Collider* c : colliders)
		{
			if (!c->isKinematic && c->hasGravity)
			{
				c->velocity += glm::vec3(0, GravityMultiplier, 0) * deltaTime;
			}
		}
	}

	std::vector<Collision> PhysicsManager::CheckIntersections(std::vector<Collider*> colliders)
	{
		std::vector<Collision> collisions;

		int count = colliders.size();

		for (int i = 0; i < count; i++)
		{
			for (int j = i + 1; j < count; j++)
			{
				Collision c = CheckIntersect(colliders[i], colliders[j]);
				if (c.collider1 != nullptr && c.collider2 != nullptr)
				{
					collisions.push_back(c);
				}
			}
		}
		return collisions;
	}
	
	bool PhysicsManager::RayCast(Ray& aRay, RayHit& aHit)
	{
		for (Collider* c : colliders) {
			if (CheckRayIntersect(aRay, c))
			{
				aHit.collider = c;
				aHit.point = glm::vec3(0, 0, 0);
				aHit.distance = 0.0f;
				return true;
			}
		}
		return false;
	}
	
	bool PhysicsManager::CheckRayIntersect(const Ray& aRay, Collider* aCollider)
	{
		if (aCollider->isOf<SphereCollider>())
		{
			SphereCollider* sphere = dynamic_cast<SphereCollider*>(aCollider);
			return RaySphereIntersect(aRay, *sphere);
		}
		else if (aCollider->isOf<BoxCollider>())
		{
			BoxCollider* box = dynamic_cast<BoxCollider*>(aCollider);
			return RayBoxIntersect(aRay, *box);
		}
		return false;
	}
	

}