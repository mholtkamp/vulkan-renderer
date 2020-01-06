#pragma once

#include "Enums.h"
#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>

#undef near
#undef far

struct OrthoSettings
{
	float mWidth;
	float mHeight;
	float mNear;
	float mFar;

	OrthoSettings() :
		mWidth(10.0f),
		mHeight(6.6f),
		mNear(0.0f),
		mFar(30.0f)
	{

	}
};

struct PerspectiveSettings
{
	float mFovY;
	float mAspectRatio;
	float mNear;
	float mFar;

	PerspectiveSettings() :
		mFovY(70.0f),
		mAspectRatio(1.78f),
		mNear(0.1f),
		mFar(1024.0f)
	{

	}
};

class Camera
{
public:

	Camera();

	~Camera() = default;

	void SetOrthoSettings(float width,
		float height,
		float near,
		float far);

	void SetPerspectiveSettings(float fovY,
		float aspectRatio,
		float near,
		float far);

	void Update();

	glm::mat4& GetViewProjectionMatrix();

	void SetPosition(glm::vec3 position);

	void SetRotation(glm::vec3 rotation);

	glm::vec3 GetPosition();

	glm::vec3 GetRotation();

private:

	ProjectionMode mProjectionMode;
	bool mTargeting;

	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mTarget;

	glm::mat4 mViewProjectMatrix;

	OrthoSettings mOrthoSettings;
	PerspectiveSettings mPerspectiveSettings;
};