#include <vulkan/vulkan.h>
#include <stdio.h>
#include <Windows.h>

#include "ApplicationInfo.h"
#include "ApplicationState.h"
#include "Renderer.h"
#include "Scene.h"
#include "CameraController.h"
#include "DebugActionHandler.h"
#include "Input.h"

static AppState sAppState;
static bool sQuit = false;
static CameraController sCameraController;
static DebugActionHandler sDebugHandler;
static Scene* sScene = nullptr;
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
	case WM_KEYDOWN:                              // Is A Key Being Held Down?
	{
		SetKey(wParam);                         // If So, Mark It As TRUE
		return 0;                                 // Jump Back
	}

	case WM_KEYUP:                                // Has A Key Been Released?
	{
		ClearKey(wParam);                        // If So, Mark It As FALSE
		return 0;                                 // Jump Back
	}

	case WM_LBUTTONDOWN:
	{
		SetButton(VBUTTON_LEFT);
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		SetButton(VBUTTON_RIGHT);
		return 0;
	}

	case WM_MBUTTONDOWN:
	{
		SetButton(VBUTTON_MIDDLE);
		return 0;
	}

	case WM_XBUTTONDOWN:
	{
		int nButton = HIWORD(wParam);

		if (nButton == 1)
		{
			SetButton(VBUTTON_X1);
		}
		else if (nButton == 2)
		{
			SetButton(VBUTTON_X2);
		}

		return 0;
	}

	case WM_LBUTTONUP:
	{
		ClearButton(VBUTTON_LEFT);
		return 0;
	}

	case WM_RBUTTONUP:
	{
		ClearButton(VBUTTON_RIGHT);
		return 0;
	}

	case WM_MBUTTONUP:
	{
		ClearButton(VBUTTON_MIDDLE);
		return 0;
	}

	case WM_XBUTTONUP:
	{
		int nButton = HIWORD(wParam);

		if (nButton == 1)
		{
			ClearButton(VBUTTON_X1);
		}
		else if (nButton == 2)
		{
			ClearButton(VBUTTON_X2);
		}

		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		SetScrollWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		int nX = LOWORD(lParam);
		int nY = HIWORD(lParam);

		// Invert the y axis to match the bottom-left origin
		// convention used thoughout Vakz.
		SetMousePosition(nX, (sAppState.mWindowHeight - 1) - nY);

		return 0;
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

void CreateNativeWindow(int32_t width, int32_t height)
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
	RECT wr = { 0, 0, width, height };
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

bool Initialize(int32_t width, int32_t height)
{
	Renderer::Create();
	Renderer* renderer = Renderer::Get();

	sAppState.mConnection = GetModuleHandle(NULL); //hInstance;

#ifdef NDEBUG
	sAppState.mValidate = false;
#else
	sAppState.mValidate = true;
#endif

	renderer->SetAppState(&sAppState);

	CreateNativeWindow(width, height);

	renderer->Initialize();

	sClock.Start();

	return true;
}

bool Update()
{
	ResetJusts(); // TODO: This needs a better name.
	ProcessMessages();
	sClock.Update();
	sDebugHandler.Update();
	sCameraController.Update(sClock.DeltaTime());
	sScene->Update(sClock.DeltaTime());
	Renderer::Get()->Render();

	return !sQuit;
}

void Shutdown()
{
	Renderer::Get()->WaitOnExecutionFinished();
	Renderer::Destroy();
	printf("Done.\n");
}

void AssignDebugCamera(Camera* camera)
{
	sCameraController.SetCamera(camera);
}

void SetScene(Scene* scene)
{
	Renderer* renderer = Renderer::Get();
	renderer->SetScene(scene);
	sScene = scene;
}

const AppState* GetAppState()
{
	return &sAppState;
}