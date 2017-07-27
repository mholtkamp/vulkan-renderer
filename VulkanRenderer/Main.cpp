#include <vulkan/vulkan.h>
#include <stdio.h>
#include <Windows.h>

#include "ApplicationInfo.h"
#include "ApplicationState.h"
#include "Renderer.h"
#include "Scene.h"
#include "CameraController.h"
#include "DebugActionHandler.h"

static AppState sAppState;
static bool sQuit = false;
static CameraController sCameraController;
static DebugActionHandler sDebugHandler;
static Clock sClock;

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(sAppState.mValidationError);
		break;
	case WM_SIZE:
	{
		WORD width = LOWORD(lParam);
		WORD height = HIWORD(lParam);

		if (width != 0 &&
			height != 0)
		{
			sAppState.mWindowWidth = width;
			sAppState.mWindowHeight = height;

			Renderer* renderer = Renderer::Get();

			if (renderer != nullptr)
			{
				renderer->RecreateSwapchain();
			}
		}
		break;
	}
	//case WM_PAINT:
	//	// The validation callback calls MessageBox which can generate paint
	//	// events - don't make more Vulkan calls if we got here from the
	//	// callback
	//	if (!sAppState.mInCallback)
	//	{
	//		if (Renderer::Get() != nullptr)
	//		{
	//			Renderer::Get()->Render();
	//		}
	//	}
	//	break;
	case WM_GETMINMAXINFO:     // set window's minimum size
		((MINMAXINFO*)lParam)->ptMinTrackSize = sAppState.mMinSize;
		return 0;
	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void CreateNativeWindow()
{
	WNDCLASSEX win_class;

	// Initialize the window class structure:
	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = WndProc;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = sAppState.mConnection; // hInstance
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = APP_NAME;
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	// Register window class:
	if (!RegisterClassEx(&win_class)) {
		// It didn't work, so try to give a useful error:
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}
	// Create window with the registered class:
	RECT wr = { 0, 0, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	sAppState.mWindow = CreateWindowEx(0,
		APP_NAME,           // class name
		APP_NAME,           // app name
		WS_OVERLAPPEDWINDOW | // window style
		WS_VISIBLE | WS_SYSMENU,
		100, 100,           // x/y coordinates
		wr.right - wr.left, // width
		wr.bottom - wr.top, // height
		NULL,               // handle to parent
		NULL,               // handle to menu
		sAppState.mConnection,   // hInstance
		NULL);              // no extra parameters
	if (!sAppState.mWindow)
	{
		// It didn't work, so try to give a useful error:
		printf("Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}
	// Window client area size must be at least 1 pixel high, to prevent crash.
	sAppState.mMinSize.x = GetSystemMetrics(SM_CXMINTRACK);
	sAppState.mMinSize.y = GetSystemMetrics(SM_CYMINTRACK) + 1;
}

void ProcessMessages()
{
	MSG     msg;
	BOOL    done = FALSE;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			sQuit = true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	Renderer::Create();
	Renderer* renderer = Renderer::Get();
	Scene* scene = new Scene();

	sAppState.mConnection = hInstance;

#ifdef NDEBUG
	sAppState.mValidate = false;
#else
	sAppState.mValidate = true;
#endif

	renderer->SetAppState(&sAppState);

	CreateNativeWindow();

	renderer->Initialize();

	//scene->Load("Scenes/MonkeyScene/Collada/", "MonkeyScene2.dae");
	//scene->Load("C:/Sponza/", "Sponza.dae");
	//scene->Load("C:/Sponza/", "Sponza_minimal.dae");
	scene->Load("D:/Courtyard/Collada/", "courtyard_minimal.dae");

	sCameraController.SetCamera(scene->GetActiveCamera());
	sClock.Start();

	renderer->SetScene(scene);

	while (sQuit == false)
	{
		ProcessMessages();
		sClock.Update();
		sDebugHandler.Update();
		sCameraController.Update(sClock.DeltaTime());
		scene->Update(sClock.DeltaTime());
		renderer->Render();
	}

	renderer->WaitOnExecutionFinished();

	delete scene;
	Renderer::Destroy();
	printf("Done.\n");
}