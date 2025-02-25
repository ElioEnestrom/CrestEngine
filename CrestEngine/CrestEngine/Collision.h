#pragma once
#include "Collider.h"

class Collision {
public:
	Collision() {}
	Collision(Collider* collider1, Collider* collider2) {
		this->collider1 = collider1;
		this->collider2 = collider2;
	}
	Collider* collider1;
	Collider* collider2;
};