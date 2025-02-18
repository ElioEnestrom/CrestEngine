#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Time.h"



namespace Camera {
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float cameraSpeed = 2.5f; // adjust accordingly
	bool firstMouse = true;
	bool cameraMoving= true;

	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;


	void MoveCamera(glm::mat4& view) {


		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void MoveForward() {
		cameraPos +=  cameraSpeed * cameraFront * Time::DeltaTime();
	}
	void MoveBackwards()
	{
		cameraPos -= cameraSpeed * cameraFront * Time::DeltaTime();
	}
	void MoveLeft()
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * Time::DeltaTime();
	}
	void MoveRight()
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * Time::DeltaTime();
	}
	void MoveUp()
	{
		cameraPos += cameraSpeed * cameraUp * Time::DeltaTime();
	}
	void MoveDown()
	{
		cameraPos -= cameraSpeed * cameraUp * Time::DeltaTime();
	}
	void ChangeMoveSpeed(float newSpeed)
	{
		cameraSpeed = newSpeed;
	}	

	void SwitchCamera() 
	{
		if (cameraMoving)
		{
			SwitchToDefault(glfwGetCurrentContext());
		}
		else
		{
			SwitchToCamera(glfwGetCurrentContext());
		}
		cameraMoving = !cameraMoving;
	}
	void SwitchToCamera(GLFWwindow* window) 
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		firstMouse = true;
	}

	void SwitchToDefault(GLFWwindow* window)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, NULL);
		glfwSetScrollCallback(window, NULL);
	}

	float GetFOV()
	{
		return fov;
	}

	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 45.0f)
			fov = 45.0f;
	}
}