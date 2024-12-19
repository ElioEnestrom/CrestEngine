#pragma once

namespace Camera {

	void MoveCamera(glm::mat4& view);
	void MoveForward();
	void MoveBackwards();
	void MoveRight();
	void MoveLeft();
	void MoveUp();
	void MoveDown();

	void ChangeMoveSpeed(float newSpeed);

	void SwitchCamera();
	void SwitchToCamera(GLFWwindow* window);
	void SwitchToDefault(GLFWwindow* window);

	float GetFOV();
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}