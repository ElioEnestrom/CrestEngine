#pragma once
#include <iostream>
#include "glm/glm.hpp"

class Collider
{
public: virtual ~Collider() {}

	  template <typename T>
	  bool isOf() {
		  return dynamic_cast<T*>(this) != NULL; }

	glm::mat4 transform;
	glm::vec3 position;

	bool hasGravity;
	glm::vec3 velocity;
	glm::vec3 center;
	float mass;
private:

};