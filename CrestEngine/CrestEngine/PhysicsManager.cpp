#include "PhysicsManager.h"
#include "SphereCollider.h"
#include "BoxCollider.h"


void PhysicsManager::SimulatePhysics(const float& deltaTime)
{
	std::vector<Collider*> colliders = UpdatePhysicsScene();

	std::vector<Collision> collisions = CheckCollisions(colliders);

	HandleCollision(collisions);

	ApplyVelocity(colliders, deltaTime);

	UpdateVisuals();
}

void PhysicsManager::ApplyVelocity(std::vector<Collider*> colliders, const float& deltaTime)
{
}

void PhysicsManager::HandleCollision(std::vector<Collision> colliders)
{
}

std::vector<Collider*> PhysicsManager::UpdatePhysicsScene()
{

	return std::vector<Collider*>();
}

std::vector<Collision> PhysicsManager::CheckCollisions(std::vector<Collider*> colliders)
{
	std::vector<Collision> collisions;
	for (Collider* collider1 : colliders) 
	{
		for (Collider* collider2 : colliders) 
		{
			if (collider1 != collider2) 
			{
				continue;
			}
			if (collider1->isOf<Collider>() && collider2->isOf<Collider>()) 
			{
				// Check for collision
				// If collision, add to collisions
				collisions.push_back(Collision(collider1, collider2));
			}
		}
	}
	return std::vector<Collision>();
}

void PhysicsManager::UpdateVisuals()
{
}

void PhysicsManager::EndStep()
{
}

bool PhysicsManager::CheckIntersect(Collider* collider1, Collider* collider2)
{
	if (collider1->isOf<SphereCollider>() && collider2->isOf<SphereCollider>())
	{
		SphereCollider* sphere1 = dynamic_cast<SphereCollider*>(collider1);
		SphereCollider* sphere2 = dynamic_cast<SphereCollider*>(collider2);
		return SphereSphereIntersect(*sphere1, *sphere2);
	}
	else if (collider1->isOf<SphereCollider>() && collider2->isOf<BoxCollider>())
	{
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider1);
		BoxCollider* box = dynamic_cast<BoxCollider*>(collider2);
		return SphereBoxIntersect(*sphere, *box);
	}
	else if (collider1->isOf<BoxCollider>() && collider2->isOf<SphereCollider>())
	{
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider2);
		BoxCollider* box = dynamic_cast<BoxCollider*>(collider1);
		return SphereBoxIntersect(*sphere, *box);
	}
	else if (collider1->isOf<BoxCollider>() && collider2->isOf<BoxCollider>())
	{
		BoxCollider* box1 = dynamic_cast<BoxCollider*>(collider1);
		BoxCollider* box2 = dynamic_cast<BoxCollider*>(collider2);
		return BoxBoxIntersect(*box1, *box2);
	}
	return false;
}

bool PhysicsManager::SphereSphereIntersect(SphereCollider& sphere1, SphereCollider& sphere2)
{
	float dist = glm::distance(sphere1.position, sphere2.position);

	if (dist < sphere1.radius + sphere2.radius)
	{
		return true;
	}
	return false;
}

bool PhysicsManager::SphereBoxIntersect(SphereCollider& sphere, BoxCollider& box)
{
	glm::vec3 sphereCenter = glm::vec3(sphere.transform[3]);
	glm::vec3 localSphereCenter = glm::inverse(box.transform) * glm::vec4(sphereCenter, 1.0f);
	glm::vec3 closestPoint = glm::clamp(localSphereCenter, -box.extents, box.extents);
	float dist2 = glm::distance(localSphereCenter, closestPoint);
	return dist2 < sphere.radius * sphere.radius;
}

bool PhysicsManager::BoxBoxIntersect(BoxCollider& box1, BoxCollider& box2)
{
	glm::mat3 rotation1 = glm::mat3(box1.transform);
	glm::mat3 rotation2 = glm::mat3(box2.transform);
	glm::vec3 translation = glm::vec3(box2.transform[3]) - glm::vec3(box1.transform[3]);
	translation = glm::transpose(rotation1) * translation;

	glm::mat3 rotation = glm::transpose(rotation1) * rotation2;
	glm::mat3 absRotation = glm::abs(rotation) + glm::mat3(0.0001f);

	glm::vec3 halfSize1 = box1.extents * 0.5f;
	glm::vec3 halfSize2 = box2.extents * 0.5f;

	//Separating Axis Theorem
	for (int i = 0; i < 3; i++)
	{
		float ra = halfSize1[i];
		float rb = glm::dot(absRotation[i], halfSize2);
		if (glm::abs(translation[i]) > ra + rb) return false;
	}

	for (int i = 0; i < 3; i++)
	{
		float ra = glm::dot(absRotation[i], halfSize1);
		float rb = halfSize2[i];
		if (glm::abs(glm::dot(rotation[i], translation)) > ra + rb) return false;
	}

	return true;
}
