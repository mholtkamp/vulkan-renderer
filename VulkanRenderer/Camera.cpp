#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

Camera::Camera() :
	mProjectionMode(ProjectionMode::PERSPECTIVE),
	mTargeting(false)
{
	
}

void Camera::SetOrthoSettings(float width, float height, float near, float far)
{
	mOrthoSettings.mWidth = width;
	mOrthoSettings.mHeight = height;
	mOrthoSettings.mNear = near;
	mOrthoSettings.mFar = far;
}

void Camera::SetPerspectiveSet(float fovx, float aspectRatio, float near, float far)
{
	mPerspectiveSettings.mFovY = fovx;
	mPerspectiveSettings.mAspectRatio = aspectRatio;
	mPerspectiveSettings.mNear = near;
	mPerspectiveSettings.mFar = far;
}

void Camera::Update()
{
	glm::mat4 view;
	glm::mat4 proj;


	view = rotate(view, radians(-mRotation.x), vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, radians(-mRotation.y), vec3(0.0f, 1.0f, 0.0f));
	view = rotate(view, radians(-mRotation.z), vec3(0.0f, 0.0f, 1.0f));

	view = translate(view, -mPosition);

	if (mProjectionMode == ProjectionMode::ORTHOGRAPHIC)
	{
		proj = ortho(-mOrthoSettings.mWidth,
			mOrthoSettings.mWidth,
			mOrthoSettings.mHeight,
			-mOrthoSettings.mHeight,
			mOrthoSettings.mNear,
			mOrthoSettings.mFar);
	}
	else
	{
		proj = perspectiveFov(radians(mPerspectiveSettings.mFovY),
			mPerspectiveSettings.mAspectRatio,
			1.0f,
			mPerspectiveSettings.mNear,
			mPerspectiveSettings.mFar);
	}

	// Needed for adjusting to NDC
	const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f);

	mViewProjectMatrix = clip * proj * view;
}

glm::mat4& Camera::GetViewProjectionMatrix()
{
	return mViewProjectMatrix;
}

void Camera::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void Camera::SetRotation(glm::vec3 rotation)
{
	mRotation = rotation;
}

glm::vec3 Camera::GetPosition()
{
	return mPosition;
}

glm::vec3 Camera::GetRotation()
{
	return mRotation;
}
