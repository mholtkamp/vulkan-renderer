#pragma once

#include "Camera.h"

class CameraController
{

public:

	CameraController();

	~CameraController();

	void Update(float deltaTime);

	void SetCamera(Camera* camera);

	Camera* GetCamera();

	void SetMoveSpeed(float moveSpeed);

	void SetRotationSpeed(float rotationSpeed);

private:

	Camera* mCamera;

	float mMoveSpeed;

	float mRotationSpeed;

};