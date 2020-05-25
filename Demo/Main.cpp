#include <stdint.h>
#include <Windows.h>

#undef min
#undef max

#include "Engine.h"
#include "Scene.h"
#include "Quad.h"
#include "Renderer.h"
#include "Text.h"

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
	Texture* testTexture = new Texture2D();
	testTexture->Load("Scenes/MonkeyScene/Blender/grid_tex.png");
	quad1->SetColor(glm::vec4(0.2, 1, 0.5, 1));
	quad1->SetTexture(testTexture);

	glm::vec4 colors[4];
	colors[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[1] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[2] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	colors[3] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	quad1->SetColor(colors);

	Quad* quad2 = new Quad();
	quad2->Create();
	quad2->SetPosition(glm::vec2(100, 200));
	quad2->SetDimensions(glm::vec2(400, 150));
	colors[1] = glm::vec4(1.0f, 0.0f, 1.0f, 0.0f);
	colors[3] = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f);
	quad2->SetColor(colors);

	Text* text1 = new Text();
	text1->Create();
	text1->SetPosition(450, 20);
	text1->SetDimensions(800, 800);
	text1->SetSize(48.0f);
	text1->SetColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	text1->SetText("Vulkan Renderer 2 Deluxe 3D");

	
	Renderer::Get()->SetRootWidget(text1);

	AssignDebugCamera(scene->GetActiveCamera());
	SetScene(scene);

	bool ret = true;

	while (ret)
	{
		ret = Update();
	}

	delete scene;
	delete quad1;
	delete quad2;
	delete testTexture;
	Shutdown();
}