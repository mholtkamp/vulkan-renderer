#include <stdint.h>
#include <Windows.h>

#undef min
#undef max

#include "Engine.h"
#include "Scene.h"

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int32_t nCmdShow)
{
	Initialize(1280, 720);

	Scene* scene = new Scene();
	scene->Load("Scenes/Sponza/", "Sponza.dae");

	AssignDebugCamera(scene->GetActiveCamera());
	SetScene(scene);

	bool ret = true;

	while (ret)
	{
		ret = Update();
	}

	delete scene;
	Shutdown();
}