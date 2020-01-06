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

    glm::vec3 GetDirection();

    glm::vec4 GetColor();

	void SetEnabled(bool enabled);

	void Update();

	glm::mat4 GetViewProjectionMatrix();

	bool ShouldCastShadows() const;

	void SetCastShadows(bool castShadows);

private:

	void GenerateViewProjectionMatrix();

private:

	glm::vec3 mPosition;
	glm::vec3 mDirection;
	glm::vec4 mColor;

	glm::mat4 mViewProjectionMatrix;

	bool mEnabled;
	bool mCastShadows;
};