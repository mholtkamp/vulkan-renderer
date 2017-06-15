#pragma once

#include "CameraController.h"
#include <Windows.h>


CameraController::CameraController() :
	mCamera(nullptr),
	mMoveSpeed(2.0f),
	mRotationSpeed(90.0f)
{

}

CameraController::~CameraController()
{

}

void CameraController::Update(float deltaTime)
{
	glm::vec3 cameraPosition = mCamera->GetPosition();
	glm::vec3 cameraRotation = mCamera->GetRotation();

	if (GetAsyncKeyState('A'))
	{
		cameraPosition.x -= mMoveSpeed * deltaTime;
	}

	if (GetAsyncKeyState('D') )
	{
		cameraPosition.x += mMoveSpeed * deltaTime;
	}

	if (GetAsyncKeyState('W'))
	{
		cameraPosition.z -= mMoveSpeed * deltaTime;
	}

	if (GetAsyncKeyState('S'))
	{
		cameraPosition.z += mMoveSpeed * deltaTime;
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		cameraRotation.y += mRotationSpeed * deltaTime;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		cameraRotation.y -= mRotationSpeed * deltaTime;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		cameraRotation.x += mRotationSpeed * deltaTime;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		cameraRotation.x -= mRotationSpeed * deltaTime;
	}

	mCamera->SetPosition(cameraPosition);
	mCamera->SetRotation(cameraRotation);
}

void CameraController::SetCamera(Camera* camera)
{
	mCamera = camera;
}

Camera* CameraController::GetCamera()
{
	return mCamera;
}

void CameraController::SetMoveSpeed(float moveSpeed)
{
	mMoveSpeed = moveSpeed;
}

void CameraController::SetRotationSpeed(float rotationSpeed)
{
	mRotationSpeed = rotationSpeed;
}

