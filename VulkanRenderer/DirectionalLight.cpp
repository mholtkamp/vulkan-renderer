#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() :
	mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mDirection(glm::vec3(0.0f, -1.0f, 0.0f)),
	mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{

}

void DirectionalLight::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void DirectionalLight::SetDirection(glm::vec3 direction)
{
	mDirection = direction;
}

void DirectionalLight::SetColor(glm::vec4 color)
{
	mColor = color;
}

void DirectionalLight::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}

void DirectionalLight::Update()
{
	GenerateViewProjectionMatrix();
}

glm::mat4 DirectionalLight::GetViewProjectionMatrix()
{
	return mViewProjectionMatrix;
}

glm::vec3 DirectionalLight::GetDirection()
{
    return mDirection;
}

glm::vec4 DirectionalLight::GetColor()
{
    return mColor;
}

void DirectionalLight::GenerateViewProjectionMatrix()
{
	glm::mat4 view;
	glm::mat4 proj;

	view = glm::lookAtRH(mPosition, mPosition + mDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::orthoRH(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);

    // Needed for adjusting to NDC
    const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);

	mViewProjectionMatrix = clip * proj * view;
}