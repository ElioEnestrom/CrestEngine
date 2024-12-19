#pragma once	

#include <GLFW/glfw3.h>


namespace Time {
		float deltaTime = 0.0f;	// Time between current frame and last frame
		float lastFrame = 0.0f; // Time of last frame
	float DeltaTime() {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		return deltaTime;
	}
}