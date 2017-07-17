#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DirectionalLight
{
public:

	DirectionalLight();

	void SetPosition(glm::vec3 position);

	void SetDirection(glm::vec3 direction);

	void SetColor(glm::vec4 color);

	void SetEnabled(bool enabled);

	void Update();

	glm::mat4 GetViewProjectionMatrix();

private:

	void GenerateViewProjectionMatrix();

private:

	bool mEnabled;
	glm::vec3 mPosition;
	glm::vec3 mDirection;
	glm::vec4 mColor;

	glm::mat4 mViewProjectionMatrix;
};