#pragma once
#include "glm/glm.hpp"

namespace Physics {

	glm::mat3 ComputeMomentOfInertiaBox(float mass, glm::vec3 extents, glm::vec3 aPosition);

	glm::mat3 ComputeMomentOfInertiaSphere(float mass, float radius);

	class Collider
	{
	public: virtual ~Collider() {}

		  template <typename T>
		  bool isOf() {
			  return dynamic_cast<T*>(this) != NULL;
		  }

		  virtual void ComputeInertia() = 0;

		  glm::mat4 transform;
		  glm::vec3 center;
		  glm::vec3 position;

		  float mass;
		  bool hasGravity;
		  glm::vec3 velocity;
		  glm::vec3	angularVelocity;

		  //move to dynamic body

		  glm::mat3	momentOfInertia;
		  glm::mat3	inverseMomentOfInertia;

		  bool isKinematic;
	private:

	};

	struct Collision 
	{
		Collider* collider1;
		Collider* collider2;

		glm::vec3 point;
		glm::vec3 normal;
	};

	class SphereCollider : public Collider
	{
	public:
		SphereCollider(const glm::vec3& aCenter, const float& aRadius) {

			velocity = glm::vec3(0.0f);
			angularVelocity = glm::vec3(0, 0, 0);
			position = glm::vec3(0, 0, 0);
			hasGravity = true;
			center = aCenter;
			radius = aRadius;
			mass = 1;


			momentOfInertia = ComputeMomentOfInertiaSphere(mass, aRadius);
			inverseMomentOfInertia = glm::inverse(momentOfInertia);
		}

		void ComputeInertia() override
		{
		}

		float radius;
	};

	class BoxCollider : public Collider {
	public:
		BoxCollider(const glm::vec3& aCenter, const glm::vec3& someExtents)
		{
			velocity = glm::vec3(0, 0, 0);
			hasGravity = false;
			center = aCenter;
			extents = someExtents;
			angularVelocity = glm::vec3(0, 0, 0);
			position = glm::vec3(0, 0, 0);
			mass = 1;
		}

		void ComputeInertia() override
		{
			momentOfInertia = ComputeMomentOfInertiaBox(mass, extents, position);
			inverseMomentOfInertia = glm::inverse(momentOfInertia);
		}

		glm::vec3 extents;
	};

	class PlaneCollider : public Collider
	{
	public:
		PlaneCollider(const glm::vec3& aNormal, float aDistance)
		{
			velocity = glm::vec3(0, 0, 0);
			angularVelocity = glm::vec3(0, 0, 0);
			position = glm::vec3(0, 0, 0);
			hasGravity = false;
			isKinematic = true;



			normal = glm::normalize(aNormal);
			distance = aDistance;
			mass = 1;
		}

		void ComputeInertia() override
		{
		}

		glm::vec3 normal;
		float distance;
	};
}