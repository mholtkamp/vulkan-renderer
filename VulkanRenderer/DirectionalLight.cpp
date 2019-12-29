#include "DirectionalLight.h"
#include "Constants.h"
#include "Renderer.h"

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

	// TODO: This is just a hack, but this will be better when component system is in place.
	// Need to grab the camera's location
	glm::vec3 cameraPosition = Renderer::Get()->GetScene()->GetActiveCamera()->GetPosition();

	view = glm::lookAtRH(cameraPosition, cameraPosition + mDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::orthoRH(-SHADOW_RANGE, SHADOW_RANGE, -SHADOW_RANGE, SHADOW_RANGE, -SHADOW_RANGE_Z, SHADOW_RANGE_Z);

    // Needed for adjusting to NDC
    const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);

	mViewProjectionMatrix = clip * proj * view;
}