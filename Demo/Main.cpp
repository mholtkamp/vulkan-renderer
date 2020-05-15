#include <stdint.h>
#include <Windows.h>

#undef min
#undef max

#include "Engine.h"
#include "Scene.h"
#include "Quad.h"
#include "Renderer.h"

#ifndef _DEBUG
int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int32_t nCmdShow)
#else
int32_t main(int32_t argc, char** argv)
#endif
{
	Initialize(1280, 720);

	Scene* scene = new Scene();
	scene->Load("Scenes/Sponza/", "Sponza.dae");
	//scene->Load("Scenes/MonkeyScene/Collada/", "MonkeyScene3.dae");

	Quad* quad1 = new Quad();
	quad1->Create();
	quad1->SetPosition(glm::vec2(100, 200));
	quad1->SetDimensions(glm::vec2(400, 150));
	//quad1->SetPosition(glm::vec2(40, 10));
	//quad1->SetDimensions(glm::vec2(1200, 700));
	quad1->SetColor(glm::vec4(1, 0, 0, 1));
	
	Renderer::Get()->SetRootWidget(quad1);

	AssignDebugCamera(scene->GetActiveCamera());
	SetScene(scene);

	bool ret = true;

	while (ret)
	{
		ret = Update();
	}

	delete scene;
	delete quad1;
	Shutdown();
}