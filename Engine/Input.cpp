#include "Input.h"
#include "Log.h"
#include "Engine.h"
#include "ApplicationState.h"
//#include "Keyboard.h"
#include <string.h>
#include <ctype.h>

static bool sKeys[VINPUT_MAX_KEYS] = { 0 };
static bool sPrevKeys[VINPUT_MAX_KEYS] = { 0 };
static bool sRepeatKeys[VINPUT_MAX_KEYS] = { 0 };
static bool sButtons[VINPUT_MAX_BUTTONS] = { 0 };
static bool sPrevButtons[VINPUT_MAX_BUTTONS] = { 0 };
static bool sTouches[VINPUT_MAX_TOUCHES] = { 0 };
static bool sPrevTouches[VINPUT_MAX_TOUCHES] = { 0 };

static int32_t sScrollWheelDelta = 0;

static int32_t sPointerX[VINPUT_MAX_TOUCHES] = { 0 };
static int32_t sPointerY[VINPUT_MAX_TOUCHES] = { 0 };

static Controller sControllers[VINPUT_MAX_CONTROLLERS];

static int32_t sNumControllers = 0;

//static Keyboard* s_pKeyboard = 0;
static bool sKeyboardEnable = 0;

static std::vector<int32_t> sJustDownKeys;

#if defined (_WIN32)
static XINPUT_STATE sXinputStates[4] = { 0 };
static XINPUT_STATE sXinputPrevStates[4] = { 0 };
static bool sActiveControllers[4] = { 0 };
#endif

void SetKey(int32_t key)
{
	if (key >= 0 &&
		key < VINPUT_MAX_KEYS)
	{
		sKeys[key] = true;
		sRepeatKeys[key] = true;
		sJustDownKeys.push_back(key);
	}
}

void UpdateInput()
{
	memcpy(sPrevKeys, sKeys, VINPUT_MAX_KEYS * sizeof(bool));
	memcpy(sPrevButtons, sButtons, VINPUT_MAX_BUTTONS * sizeof(bool));
	memcpy(sPrevTouches, sTouches, VINPUT_MAX_TOUCHES * sizeof(bool));
	memset(sRepeatKeys, 0, VINPUT_MAX_KEYS * sizeof(bool));

	sJustDownKeys.clear();

	sScrollWheelDelta = 0;
}

void ClearKey(int32_t key)
{
	if (key >= 0 &&
		key < VINPUT_MAX_KEYS)
	{
		sKeys[key] = false;
	}
}

void ClearAllKeys()
{
	int32_t i = 0;

	// Do not clear hardware keys
	int32_t back = sKeys[VKEY_BACK];

	for (i = 0; i < VINPUT_MAX_KEYS; i++)
	{
		sKeys[i] = false;
	}

	// Restore hardware keys
	sKeys[VKEY_BACK] = back;
}

bool IsKeyDown(int32_t key)
{
	if (key >= 0 &&
		key < VINPUT_MAX_KEYS)
	{
		return sKeys[key];
	}
	else
	{
		LogWarning("Invalid key queried in IsKeyDown().");
		return false;
	}
}

bool IsKeyJustDownRepeat(int32_t key)
{
	if (key >= 0 &&
		key < VINPUT_MAX_KEYS)
	{
		return sRepeatKeys[key];
	}
	else
	{
		LogWarning("Invalid key queried in IsKeyDown().");
		return false;
	}
}

bool IsKeyJustDown(int32_t key)
{
	if (key >= 0 &&
		key < VINPUT_MAX_KEYS)
	{
		return sKeys[key] && !sPrevKeys[key];
	}
	else
	{
		LogWarning("Invalid key queried in IsKeyJustDown().");
		return false;
	}
}

bool IsKeyJustUp(int32_t key)
{
	if (key >= 0 &&
		key < VINPUT_MAX_KEYS)
	{
		return sPrevKeys[key] && !sKeys[key];
	}
	else
	{
		LogWarning("Invalid key queried in IsKeyJustUp().");
		return false;
	}
}

const std::vector<int32_t>& GetJustDownKeys()
{
	return sJustDownKeys;
}

void SetButton(int32_t button)
{
	if (button >= 0 &&
		button < VINPUT_MAX_BUTTONS)
	{
		sButtons[button] = true;

		if (button == VBUTTON_LEFT)
		{
			sTouches[0] = true;
		}
	}
}

void ClearButton(int32_t button)
{
	if (button >= 0 &&
		button < VINPUT_MAX_BUTTONS)
	{
		sButtons[button] = false;

		if (button == VBUTTON_LEFT)
		{
			sTouches[0] = false;
		}
	}
}

bool IsButtonDown(int32_t button)
{
	if (button >= 0 &&
		button < VINPUT_MAX_BUTTONS)
	{
		return sButtons[button];
	}
	else
	{
		LogWarning("Invalid button queried in IsButtonDown().");
		return false;
	}
}

bool IsButtonJustDown(int32_t button)
{
	if (button >= 0 &&
		button < VINPUT_MAX_BUTTONS)
	{
		return sButtons[button] && !sPrevButtons[button];
	}
	else
	{
		LogWarning("Invalid button queried in IsButtonJustDown().");
		return false;
	}
}

bool IsButtonJustUp(int32_t button)
{
	if (button >= 0 &&
		button < VINPUT_MAX_BUTTONS)
	{
		return !sButtons[button] && sPrevButtons[button];
	}
	else
	{
		LogWarning("Invalid button queried in IsButtonJustUp().");
		return false;
	}
}

void SetTouch(int32_t touch)
{
	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		sTouches[touch] = true;
	}
	else
	{
		LogWarning("Invalid touch index in SetTouch()");
	}
}

void ClearTouch(int32_t touch)
{
	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		sTouches[touch] = false;
	}
}

bool IsTouchDown(int32_t touch)
{
	float x = 0.0f;
	float y = 0.0f;

	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		if (sKeyboardEnable)
		{
			GetPointerPositionNormalized(x, y, touch);
			if (y > 0.0f)
			{
				return sTouches[touch];
			}
			else
			{
				return false;
			}
		}
		else
		{
			return sTouches[touch];
		}
	}
	else
	{
		LogWarning("Invalid touch queried in IsTouchDown().");
		return false;
	}
}

bool IsPointerDown(int32_t pointer)
{
	float x = 0.0f;
	float y = 0.0f;

	if (pointer >= 0 &&
		pointer < VINPUT_MAX_TOUCHES)
	{
		// If either the left mouse button is down or the specified
		// touch index is down, then return 1.
		if (sTouches[pointer])
		{
			if (sKeyboardEnable)
			{
				GetPointerPositionNormalized(x, y, pointer);
				if (y > 0.0f)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// Return true if down and no keyboard.
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		LogWarning("Invalid pointer queried in IsPointerDown().");
		return false;
	}
}

bool IsPointerJustUp(int32_t pointer)
{
	float x = 0.0f;
	float y = 0.0f;

	if (pointer >= 0 &&
		pointer < VINPUT_MAX_TOUCHES)
	{
		// If either the left mouse button is down or the specified
		// touch index is down, then return 1.
		if (!sTouches[pointer] && sPrevTouches[pointer])
		{
			if (sKeyboardEnable)
			{
				GetPointerPositionNormalized(x, y, pointer);
				if (y > 0.0f)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// Return true if down and no keyboard.
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		LogWarning("Invalid pointer queried in IsPointerJustUp().");
		return false;
	}
}

bool IsPointerJustDown(int32_t pointer)
{
	float x = 0.0f;
	float y = 0.0f;

	if (pointer >= 0 &&
		pointer < VINPUT_MAX_TOUCHES)
	{
		// If either the left mouse button is down or the specified
		// touch index is down, then return 1.
		if (sTouches[pointer] && !sPrevTouches[pointer])
		{
			if (sKeyboardEnable)
			{
				GetPointerPositionNormalized(x, y, pointer);
				if (y > 0.0f)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// Return true if down and no keyboard.
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		LogWarning("Invalid pointer queried in IsPointerJustDown().");
		return false;
	}
}

void GetMousePosition(int32_t& mouseX, int32_t& mouseY)
{
	// First pointer location is for mouse.
	mouseX = sPointerX[0];
	mouseY = sPointerY[0];
}

void GetTouchPosition(int32_t& touchX,
	int32_t& touchY,
	int32_t touch)
{
	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		touchX = sPointerX[touch];
		touchY = sPointerY[touch];
	}
	else
	{
		LogWarning("Invalid touch index queried in GetTouchPosition().");
		return;
	}
}

void GetTouchPositionNormalized(float& fTouchX,
	float& fTouchY,
	int32_t touch)
{
	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		const AppState* appState = GetAppState();
		fTouchX = (sPointerX[touch] - (appState->mWindowWidth / 2.0f)) / (appState->mWindowWidth / 2.0f);
		fTouchY = (sPointerY[touch] - (appState->mWindowHeight / 2.0f)) / (appState->mWindowHeight / 2.0f);
	}
	else
	{
		LogWarning("Invalid touch index queried in GetTouchPosition().");
		return;
	}
}

void GetPointerPosition(int32_t& pointerX,
	int32_t& pointerY,
	int32_t pointer)
{
	GetTouchPosition(pointerX,
		pointerY,
		pointer);
}

void GetPointerPositionNormalized(float& pointerX,
	float& pointerY,
	int32_t pointer)
{
	GetTouchPositionNormalized(pointerX,
		pointerY,
		pointer);
}

void SetMousePosition(int32_t mouseX, int32_t mouseY)
{
	// First index in pointer array is mouse position.
	sPointerX[0] = mouseX;
	sPointerY[0] = mouseY;
}

void SetTouchPosition(int32_t touchX,
	int32_t touchY,
	int32_t touch)
{
	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		sPointerX[touch] = touchX;
		sPointerY[touch] = touchY;
	}
	else
	{
		LogWarning("Invalid touch index in SetTouchPosition().");
		return;
	}
}

void SetControllerButton(int32_t controllerButton,
	int32_t controllerNumber)
{
#if !defined(_WIN32)
	if (controllerNumber >= 0 &&
		controllerNumber < VINPUT_MAX_CONTROLLERS)
	{
		if (controllerButton >= VCONT_A &&
			controllerButton < VCONT_SELECT)
		{
			// Bias the controller button by VCONT_A to get correct array index
			sControllers[controllerNumber].mButtons[controllerButton - VCONT_A] = true;
		}
	}
#endif
}

void ClearControllerButton(int32_t controllerButton,
	int32_t controllerNumber)
{
#if !defined(_WIN32)
	if (controllerNumber >= 0 &&
		controllerNumber < VINPUT_MAX_CONTROLLERS)
	{
		if (controllerButton >= VCONT_A &&
			controllerButton < VCONT_SELECT)
		{
			// Bias the controller button by VCONT_A to get correct array index
			sControllers[controllerNumber].mButtons[controllerButton - VCONT_A] = false;
		}
	}
#endif
}

bool IsControllerButtonDown(int32_t controllerButton,
	int32_t controllerNumber)
{
#if defined(_WIN32)

	if (sXinputStates[controllerNumber].Gamepad.wButtons & controllerButton)
	{
		return true;
	}
	else
	{
		return false;
	}

#else
	if (controllerNumber >= 0 &&
		controllerNumber < VINPUT_MAX_CONTROLLERS)
	{
		if (controllerButton >= VCONT_A &&
			controllerButton < VCONT_SELECT)
		{
			// Bias the controller button by VCONT_A to get correct array index
			return sControllers[controllerNumber].mButtons[controllerButton - VCONT_A];
		}
	}

	return false;
#endif
}

bool IsControllerButtonJustDown(int32_t controllerButton,
	int32_t controllerNumber)
{
#if defined (_WIN32)
	if ((sXinputStates[controllerNumber].Gamepad.wButtons & controllerButton) &&
		!(sXinputPrevStates[controllerNumber].Gamepad.wButtons & controllerButton))
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif
}

void SetControllerAxisValue(int32_t controllerAxis,
	float fAxisValue,
	int32_t controllerNumber)
{
#if !defined(_WIN32)
	if (controllerNumber >= 0 &&
		controllerNumber < VINPUT_MAX_CONTROLLERS)
	{
		if (controllerAxis >= VCONT_AXIS_X &&
			controllerAxis < VCONT_AXIS_RTRIGGER)
		{
			sControllers[controllerNumber].mAxes[controllerAxis] = fAxisValue;
		}
	}
#endif
}

float GetControllerAxisValue(int32_t controllerAxis,
	int32_t controllerNumber)
{
#if defined (_WIN32)
	float ret = 0.0f;

	// Is the controller connected? if not 0.0f
	if (!IsControllerConnected(controllerNumber))
	{
		return 0.0f;
	}

	switch (controllerAxis)
	{
	case VCONT_AXIS_LTRIGGER:
		ret = (float)sXinputStates[controllerNumber].Gamepad.bLeftTrigger / 255;
		break;
	case VCONT_AXIS_RTRIGGER:
		ret = (float)sXinputStates[controllerNumber].Gamepad.bRightTrigger / 255;
		break;
	case VCONT_AXIS_LTHUMB_X:
		ret = (float)sXinputStates[controllerNumber].Gamepad.sThumbLX / 32767;
		break;
	case VCONT_AXIS_LTHUMB_Y:
		ret = (float)sXinputStates[controllerNumber].Gamepad.sThumbLY / 32767;
		break;
	case VCONT_AXIS_RTHUMB_X:
		ret = (float)sXinputStates[controllerNumber].Gamepad.sThumbRX / 32767;
		break;
	case VCONT_AXIS_RTHUMB_Y:
		ret = (float)sXinputStates[controllerNumber].Gamepad.sThumbRY / 32767;
		break;
	default:
		break;
	}

	return ret;
#else
	if (controllerNumber >= 0 &&
		controllerNumber < VINPUT_MAX_CONTROLLERS)
	{
		if (controllerAxis >= VCONT_AXIS_X &&
			controllerAxis < VCONT_AXIS_RTRIGGER)
		{
			return sControllers[controllerNumber].mAxes[controllerAxis];
		}
	}

	return 0.0f;
#endif
}

int32_t GetControllerIndex(int32_t inputDevice)
{
	int32_t i = 0;

	for (i = 0; i < sNumControllers; i++)
	{
		if (sControllers[i].mDevice == inputDevice)
		{
			return i;
		}
	}

	if (i < VINPUT_MAX_CONTROLLERS)
	{
		AssignController(inputDevice);
		return i;
	}

	return -1;
}

void AssignController(int32_t inputDevice)
{
#if !defined(_WIN32)
	if (sNumControllers < VINPUT_MAX_CONTROLLERS)
	{
		sControllers[sNumControllers].mDevice = inputDevice;
		sNumControllers++;
	}
#endif
}

bool IsControllerConnected(int32_t index)
{
#if defined (_WIN32)
	return sActiveControllers[index];
#else
	if (index < sNumControllers)
	{
		return true;
	}
	else
	{
		return false;
	}
#endif
}

void RefreshControllerStates()
{
#if defined (_WIN32)
	memcpy(sXinputPrevStates, sXinputStates, sizeof(XINPUT_STATE) * VINPUT_MAX_CONTROLLERS);
	memset(sXinputStates, 0, sizeof(XINPUT_STATE) * VINPUT_MAX_CONTROLLERS);

	for (int32_t i = 0; i < XUSER_MAX_COUNT; i++)
	{
		if (!XInputGetState(i, &sXinputStates[i]))
		{
			sActiveControllers[i] = true;
		}
		else
		{
			sActiveControllers[i] = false;
		}
	}

#endif
}

void ShowSoftKeyboard()
{
	sKeyboardEnable = true;
}

void HideSoftKeyboard()
{
	sKeyboardEnable = false;
}

void InitializeSoftKeyboard()
{
	//if (s_pKeyboard == nullptr)
	//{
	//	s_pKeyboard = new Keyboard();
	//}
}

bool IsSoftKeyboardEnabled()
{
	return sKeyboardEnable;
}

void RenderSoftKeyboard()
{
	//if (s_pKeyboard &&
	//	s_nKeyboardEnable)
	//{
	//	s_pKeyboard->Render();
	//}
	//else
	//{
	//	LogWarning("Soft Keyboard not initialized.");
	//}
}

void UpdateSoftKeyboard()
{
	//if (s_pKeyboard &&
	//	s_nKeyboardEnable)
	//{
	//	s_pKeyboard->Update();
	//}
}

bool IsPointerDownRaw(int32_t pointer)
{
	if (pointer >= 0 &&
		pointer < VINPUT_MAX_TOUCHES)
	{
		// If either the left mouse button is down or the specified
		// touch index is down, then return 1.
		return (sButtons[VBUTTON_LEFT] || sTouches[pointer]);
	}

	return false;
}

bool IsTouchDownRaw(int32_t touch)
{
	if (touch >= 0 &&
		touch < VINPUT_MAX_TOUCHES)
	{
		return sTouches[touch];
	}

	return false;
}

int32_t CharToKey(char cTarget)
{
	cTarget = toupper(cTarget);

#if defined (ANDROID)
	if (cTarget >= '0' &&
		cTarget <= '9')
	{
		return VKEY_0 + (cTarget - '0');
	}
	else if (cTarget >= 'A' &&
		cTarget <= 'Z')
	{
		return VKEY_A + (cTarget - 'A');
	}
#elif defined (_WIN32)
	return static_cast<int32_t>(cTarget);
#endif

	return 0;
}

void SetScrollWheelDelta(int32_t nDelta)
{
	sScrollWheelDelta = nDelta;
}

int32_t GetScrollWheelDelta()
{
	return sScrollWheelDelta;
}