#include <stdint.h>
#include <Windows.h>

#undef min
#undef max

#include "Engine.h"
#include "Scene.h"
#include "Quad.h"
#include "Button.h"
#include "Selector.h"
#include "CheckBox.h"
#include "Renderer.h"
#include "Text.h"
#include "Canvas.h"
#include "Log.h"
#include "DefaultFonts.h"

static Text* fontDemoText = nullptr;
static Text* fontNameText = nullptr;
static Canvas* fontTestCanvas = nullptr;

static float fontDemoSize = 32.0f;

static Font* demoFonts[] =
{
	&DefaultFonts::sRoboto32,
	&DefaultFonts::sUbuntu32,
	&DefaultFonts::sPressStart16,
	&DefaultFonts::sRoboto32_DF,
	&DefaultFonts::sRobotoMono24,
	&DefaultFonts::sUbuntuMono24
};

static int32_t numFonts = ARRAYSIZE(demoFonts);
static int32_t currentFont = 0;

void PlusSize(Button* button)
{
	const float increment = 4.0f;
	fontDemoSize += increment;
	fontDemoText->SetSize(fontDemoSize);
}

void MinusSize(Button* button)
{
	const float decrement = 4.0f;
	fontDemoSize -= decrement;
	fontDemoSize = glm::max(fontDemoSize, 4.0f);
	fontDemoText->SetSize(fontDemoSize);
}

void CycleFont(Button* button)
{
	int32_t fontIndex = static_cast<Selector*>(button)->GetSelectionIndex();
	Font* font = demoFonts[fontIndex];
	fontNameText->SetText(font->mName);
	fontDemoText->SetFont(font);
}

void ShowFontTest(Button* button)
{
	CheckBox* check = static_cast<CheckBox*>(button);
	fontTestCanvas->SetVisible(check->IsChecked());
}

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

	Canvas* rootCanvas = new Canvas();
	rootCanvas->SetRect(0, 0, 1280, 720);

	Canvas* canvas2 = new Canvas();
	canvas2->SetRect(50, 570, 100, 100);

	fontTestCanvas = new Canvas();
	fontTestCanvas->SetRect(0, 0, 1280, 720);

	Quad* quad1 = new Quad();
	quad1->SetPosition(glm::vec2(-150, -150));
	quad1->SetDimensions(glm::vec2(400, 400));
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
	quad2->SetPosition(glm::vec2(0, 0));
	quad2->SetDimensions(glm::vec2(1280, 100));
	colors[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);
	colors[1] = glm::vec4(1.0f, 0.0f, 1.0f, 0.0f);
	colors[2] = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f);
	colors[3] = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f);
	quad2->SetColor(colors);

	Text* text1 = new Text();
	text1->SetPosition(450, 20);
	text1->SetDimensions(800, 800);
	text1->SetSize(48.0f);
	text1->SetText("Vulkan Renderer 2 Deluxe 3D");

	fontNameText = new Text();
	fontNameText->SetPosition(100, 250);
	fontNameText->SetDimensions(1000, 400);
	fontNameText->SetSize(48);
	fontNameText->SetText(demoFonts[currentFont]->mName);

	fontDemoText = new Text();
	fontDemoText->SetPosition(100, 300);
	fontDemoText->SetDimensions(1000, 400);
	fontDemoText->SetSize(fontDemoSize);
	fontDemoText->SetText("Beep Boop!\nThis is a font test.\nThe quick brown fox jumps over the lazy dog?");

	Button* buttonMinus = new Button();
	buttonMinus->GetText()->SetText("  -");
	buttonMinus->SetPosition(100, 200);
	buttonMinus->SetDimensions(32, 32);
	buttonMinus->SetPressedHandler(MinusSize);

	Button* buttonPlus = new Button();
	buttonPlus->GetText()->SetText("  +");
	buttonPlus->SetPosition(140, 200);
	buttonPlus->SetDimensions(32, 32);
	buttonPlus->SetPressedHandler(PlusSize);

	CheckBox* enableCheckbox = new CheckBox();
	enableCheckbox->SetPressedHandler(ShowFontTest);
	enableCheckbox->SetChecked(true);
	enableCheckbox->SetPosition(1100, 683);

	Text* enableLabel = new Text();
	enableLabel->SetText("Enable Font Test");
	enableLabel->SetPosition(1130, 685);

	Selector* selectorFont = new Selector();
	for (int32_t i = 0; i < numFonts; ++i)
	{
		selectorFont->AddSelection(demoFonts[i]->mName);
	}
	selectorFont->SetPosition(180, 200);
	selectorFont->SetDimensions(180, 32);
	selectorFont->SetPressedHandler(CycleFont);

	canvas2->AddChild(quad1);

	fontTestCanvas->AddChild(buttonMinus);
	fontTestCanvas->AddChild(buttonPlus);
	fontTestCanvas->AddChild(selectorFont);
	fontTestCanvas->AddChild(fontNameText);
	fontTestCanvas->AddChild(fontDemoText);

	rootCanvas->AddChild(enableCheckbox);
	rootCanvas->AddChild(enableLabel);
	rootCanvas->AddChild(quad2);
	rootCanvas->AddChild(text1);
	rootCanvas->AddChild(canvas2);
	rootCanvas->AddChild(fontTestCanvas);

	Renderer::Get()->SetRootWidget(rootCanvas);

	AssignDebugCamera(scene->GetActiveCamera());
	SetScene(scene);

	bool ret = true;

	while (ret)
	{
		ret = Update();
	}

	delete rootCanvas;
	delete scene;
	delete testTexture;

	Shutdown();
}