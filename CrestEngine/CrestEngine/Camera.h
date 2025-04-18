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
	glm::vec3 GetCameraPos();

	glm::vec2 GetCameraRotation();
	glm::vec3 GetCameraFront();
	void SetCameraRotation(float newYaw, float newPitch);

	void SetCameraPos(glm::vec3 newPos);

	void SwitchCamera();
	void SwitchToCamera(GLFWwindow* window);
	void SwitchToDefault(GLFWwindow* window);

	float GetFOV();
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}