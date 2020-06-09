#pragma once

#include <stdint.h>

bool Initialize(int32_t width, int32_t height);

bool Update();

void Shutdown();

void AssignDebugCamera(class Camera* camera);

void SetScene(class Scene* scene);

const struct AppState* GetAppState();
