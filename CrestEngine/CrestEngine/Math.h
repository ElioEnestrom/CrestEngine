#pragma once
#include "glm/glm.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace Math {

	//float Distance(const glm::vec3& a, const glm::vec3& b)
	//{
	//	return glm::distance(a, b);
	//}
	//float Distance2(const glm::vec3& a, const glm::vec3& b)
	//{
	//	return glm::distance2(a, b);
	//}
	//glm::vec3 Normalize(const glm::vec3& a)
	//{
	//	return glm::normalize(a);
	//}
	//glm::vec3 Clamp(const glm::vec3& value, const glm::vec3& min, const glm::vec3& max)
	//{
	//	return glm::clamp(value, min, max);
	//}
	//glm::vec3 Cross(const glm::vec3& a, const glm::vec3& b)
	//{
	//	return glm::cross(a, b);
	//}
	//float Dot(const glm::vec3& a, const glm::vec3& b)
	//{
	//	return glm::dot(a, b);
	//}
	//glm::vec3 Reflect(const glm::vec3& incident, const glm::vec3& normal)
	//{
	//	return glm::reflect(incident, normal);
	//}
	//glm::vec3 Refract(const glm::vec3& incident, const glm::vec3& normal, const float& eta)
	//{
	//	return glm::refract(incident, normal, eta);
	//}
	//glm::vec3 Transform(const glm::vec3& vec, const glm::mat4& mat)
	//{
	//	return glm::vec3(mat * glm::vec4(vec, 1.0f));
	//}
	//glm::vec3 TransformNormal(const glm::vec3& vec, const glm::mat4& mat)
	//{
	//	return glm::vec3(mat * glm::vec4(vec, 0.0f));
	//}
	//glm::vec3 TransformDirection(const glm::vec3& vec, const glm::mat4& mat)
	//{
	//	return glm::normalize(glm::vec3(mat * glm::vec4(vec, 0.0f)));
	//}
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	
	//glm::mat4 LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
	//glm::mat4 Perspective(const float& fovy, const float& aspect, const float& near, const float& far);
	//glm::mat4 Orthographic(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far);
	//glm::mat4 Rotate(const glm::vec3& eulerAngles);
}