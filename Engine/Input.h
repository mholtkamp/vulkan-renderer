#pragma once

#include <string.h>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <Xinput.h>

enum VakzKeyEnum
{
	VKEY_BACK = 10,

	VKEY_0 = 48,
	VKEY_1 = 49,
	VKEY_2 = 50,
	VKEY_3 = 51,
	VKEY_4 = 52,
	VKEY_5 = 53,
	VKEY_6 = 54,
	VKEY_7 = 55,
	VKEY_8 = 56,
	VKEY_9 = 57,

	VKEY_A = 65,
	VKEY_B = 66,
	VKEY_C = 67,
	VKEY_D = 68,
	VKEY_E = 69,
	VKEY_F = 70,
	VKEY_G = 71,
	VKEY_H = 72,
	VKEY_I = 73,
	VKEY_J = 74,
	VKEY_K = 75,
	VKEY_L = 76,
	VKEY_M = 77,
	VKEY_N = 78,
	VKEY_O = 79,
	VKEY_P = 80,
	VKEY_Q = 81,
	VKEY_R = 82,
	VKEY_S = 83,
	VKEY_T = 84,
	VKEY_U = 85,
	VKEY_V = 86,
	VKEY_W = 87,
	VKEY_X = 88,
	VKEY_Y = 89,
	VKEY_Z = 90,

	VKEY_SPACE = 32,
	VKEY_ENTER = 13,
	VKEY_BACKSPACE = 8,
	VKEY_TAB = 9,

	VKEY_SHIFT = 16,
	VKEY_CONTROL = 17,

	VKEY_UP = 38,
	VKEY_DOWN = 40,
	VKEY_LEFT = 37,
	VKEY_RIGHT = 39,

	VKEY_NUMPAD0 = 96,
	VKEY_NUMPAD1 = 97,
	VKEY_NUMPAD2 = 98,
	VKEY_NUMPAD3 = 99,
	VKEY_NUMPAD4 = 100,
	VKEY_NUMPAD5 = 101,
	VKEY_NUMPAD6 = 102,
	VKEY_NUMPAD7 = 103,
	VKEY_NUMPAD8 = 104,
	VKEY_NUMPAD9 = 105,

	VKEY_F1 = 112,
	VKEY_F2 = 113,
	VKEY_F3 = 114,
	VKEY_F4 = 115,
	VKEY_F5 = 116,
	VKEY_F6 = 117,
	VKEY_F7 = 118,
	VKEY_F8 = 119,
	VKEY_F9 = 120,
	VKEY_F10 = 121,
	VKEY_F11 = 122,
	VKEY_F12 = 123,

	VKEY_PERIOD = 0xBE,
	VKEY_COMMA = 0xBC,
	VKEY_PLUS = 0xBB,
	VKEY_MINUS = 0xBD,

	VKEY_COLON = 0xBA,
	VKEY_QUESTION = 0xBF,
	VKEY_SQUIGGLE = 0xC0,
	VKEY_LEFT_BRACKET = 0xDB,
	VKEY_BACK_SLASH = 0xDC,
	VKEY_RIGHT_BRACKET = 0xDD,
	VKEY_QUOTE = 0xDE,

	VKEY_DELETE = 0x2E

};

enum VakzButtonEnum
{
	VBUTTON_LEFT = 0,
	VBUTTON_RIGHT = 1,
	VBUTTON_MIDDLE = 2,
	VBUTTON_X1 = 3,
	VBUTTON_X2 = 4
};

enum VakzControllerEnum
{
	VCONT_A = XINPUT_GAMEPAD_A,
	VCONT_B = XINPUT_GAMEPAD_B,
	VCONT_C = 0,
	VCONT_X = XINPUT_GAMEPAD_X,
	VCONT_Y = XINPUT_GAMEPAD_Y,
	VCONT_Z = 0,
	VCONT_L1 = XINPUT_GAMEPAD_LEFT_SHOULDER,
	VCONT_R1 = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	VCONT_L2 = 0,
	VCONT_R2 = 0,
	VCONT_THUMBL = XINPUT_GAMEPAD_LEFT_THUMB,
	VCONT_THUMBR = XINPUT_GAMEPAD_RIGHT_THUMB,
	VCONT_START = XINPUT_GAMEPAD_START,
	VCONT_SELECT = XINPUT_GAMEPAD_BACK,

	VCONT_AXIS_X = 0,
	VCONT_AXIS_Y = 0,
	VCONT_AXIS_Z = 0,
	VCONT_AXIS_RZ = 0,
	VCONT_AXIS_HAT_X = 0,
	VCONT_AXIS_HAT_Y = 0,

	VCONT_AXIS_LTRIGGER = 1,
	VCONT_AXIS_RTRIGGER = 2,
	VCONT_AXIS_LTHUMB_X = 3,
	VCONT_AXIS_LTHUMB_Y = 4,
	VCONT_AXIS_RTHUMB_X = 5,
	VCONT_AXIS_RTHUMB_Y = 6
};

#elif defined(ANDROID)
enum VakzKeyEnum
{

	VKEY_BACK = 4,

	VKEY_0 = 7,
	VKEY_1 = 8,
	VKEY_2 = 9,
	VKEY_3 = 10,
	VKEY_4 = 11,
	VKEY_5 = 12,
	VKEY_6 = 13,
	VKEY_7 = 14,
	VKEY_8 = 15,
	VKEY_9 = 16,

	VKEY_A = 29,
	VKEY_B = 30,
	VKEY_C = 31,
	VKEY_D = 32,
	VKEY_E = 33,
	VKEY_F = 34,
	VKEY_G = 35,
	VKEY_H = 36,
	VKEY_I = 37,
	VKEY_J = 38,
	VKEY_K = 39,
	VKEY_L = 40,
	VKEY_M = 41,
	VKEY_N = 42,
	VKEY_O = 43,
	VKEY_P = 44,
	VKEY_Q = 45,
	VKEY_R = 46,
	VKEY_S = 47,
	VKEY_T = 48,
	VKEY_U = 49,
	VKEY_V = 50,
	VKEY_W = 51,
	VKEY_X = 52,
	VKEY_Y = 53,
	VKEY_Z = 54,

	VKEY_SPACE = 62,
	VKEY_ENTER = 66,
	VKEY_BACKSPACE = 67,
	VKEY_TAB = 61,

	VKEY_SHIFT = 60,
	VKEY_CONTROL = 113,

	VKEY_UP = 19,
	VKEY_DOWN = 20,
	VKEY_LEFT = 21,
	VKEY_RIGHT = 22,

	VKEY_NUMPAD0 = 144,
	VKEY_NUMPAD1 = 145,
	VKEY_NUMPAD2 = 146,
	VKEY_NUMPAD3 = 147,
	VKEY_NUMPAD4 = 148,
	VKEY_NUMPAD5 = 149,
	VKEY_NUMPAD6 = 150,
	VKEY_NUMPAD7 = 151,
	VKEY_NUMPAD8 = 152,
	VKEY_NUMPAD9 = 153,

	VKEY_F1 = 131,
	VKEY_F2 = 132,
	VKEY_F3 = 133,
	VKEY_F4 = 134,
	VKEY_F5 = 135,
	VKEY_F6 = 136,
	VKEY_F7 = 137,
	VKEY_F8 = 138,
	VKEY_F9 = 139,
	VKEY_F10 = 140,
	VKEY_F11 = 141,
	VKEY_F12 = 142,

	VKEY_PERIOD = 56,
	VKEY_COMMA = 55,
	VKEY_PLUS = 70,
	VKEY_MINUS = 69,
	VKEY_COLON = 74,
	VKEY_QUESTION = 76,
	VKEY_SQUIGGLE = 216, // Couldnt find keycode
	VKEY_LEFT_BRACKET = 71,
	VKEY_BACK_SLASH = 73,
	VKEY_RIGHT_BRACKET = 72,
	VKEY_QUOTE = 218, // Couldnt find keycode

	VKEY_DELETE = 67
};

enum VakzButtonEnum
{
	VBUTTON_LEFT = 0,
	VBUTTON_RIGHT = 1,
	VBUTTON_MIDDLE = 2,
	VBUTTON_X1 = 3,
	VBUTTON_X2 = 4
};

enum VakzControllerEnum
{
	VCONT_A = 96,
	VCONT_B = 97,
	VCONT_C = 98,
	VCONT_X = 99,
	VCONT_Y = 100,
	VCONT_Z = 101,
	VCONT_L1 = 102,
	VCONT_R1 = 103,
	VCONT_L2 = 104,
	VCONT_R2 = 105,
	VCONT_THUMBL = 106,
	VCONT_THUMBR = 107,
	VCONT_START = 108,
	VCONT_SELECT = 109,

	VCONT_AXIS_X = 0,
	VCONT_AXIS_Y = 1,
	VCONT_AXIS_Z = 11,
	VCONT_AXIS_RZ = 14,
	VCONT_AXIS_HAT_X = 15,
	VCONT_AXIS_HAT_Y = 16,
	VCONT_AXIS_LTRIGGER = 17,
	VCONT_AXIS_RTRIGGER = 18
};

#endif

enum VInputEnum
{
	VINPUT_MAX_KEYS = 256,
	VINPUT_MAX_BUTTONS = 16,
	VINPUT_MAX_TOUCHES = 4,
	VINPUT_MAX_CONTROLLERS = 4,
	VINPUT_CONT_BUTTONS = 14,
	VINPUT_CONT_AXES = 18
};

class Controller
{
public:
	Controller()
	{
		mDevice = -1;
		memset(mButtons, 0, VINPUT_CONT_BUTTONS * sizeof(int32_t));
		memset(mAxes, 0, VINPUT_CONT_AXES * sizeof(float));
	}

	int32_t   mDevice;
	int32_t   mButtons[VINPUT_CONT_BUTTONS];
	float mAxes[VINPUT_CONT_AXES];
};

void SetKey(int32_t key);
void ClearKey(int32_t key);

void UpdateInput();

void ClearAllKeys();

bool IsKeyDown(int32_t key);
bool IsKeyJustDownRepeat(int32_t key);
bool IsKeyJustDown(int32_t key);
bool IsKeyJustUp(int32_t key);

const std::vector<int32_t>& GetJustDownKeys();

void SetButton(int32_t button);
void ClearButton(int32_t button);
bool IsButtonDown(int32_t button);

bool IsButtonJustDown(int32_t button);
bool IsButtonJustUp(int32_t button);

void SetTouch(int32_t touch);
void ClearTouch(int32_t touch);
bool IsTouchDown(int32_t touch);

bool IsPointerJustUp(int32_t pointer = 0);
bool IsPointerJustDown(int32_t pointer = 0);
bool IsPointerDown(int32_t pointer = 0);

void GetMousePosition(int32_t& mouseX, int32_t& mouseY);
void GetTouchPosition(int32_t& touchX, int32_t& touchY, int32_t touch);
void GetTouchPositionNormalized(float& fTouchX, float& fTouchY, int32_t touch);
void GetPointerPosition(int32_t& pointerX, int32_t& pointerY, int32_t pointer = 0);
void GetPointerPositionNormalized(float& fPointerX, float& fPointerY, int32_t  pointer = 0);

void SetMousePosition(int32_t mouseX, int32_t mouseY);
void SetTouchPosition(int32_t touchX, int32_t touchY, int32_t touch = 0);

void SetControllerButton(int32_t controllerButton, int32_t controllerNumber);
void ClearControllerButton(int32_t controllerButton, int32_t controllerNumber);
bool IsControllerButtonDown(int32_t controllerButton, int32_t controllerNumber);
bool IsControllerButtonJustDown(int32_t controllerButton, int32_t controllerNumber);

void SetControllerAxisValue(int32_t controllerAxis, float fAxisValue, int32_t controllerNumber);
float GetControllerAxisValue(int32_t controllerAxis, int32_t controllerNumber);

int32_t GetControllerIndex(int32_t inputDevice);
void AssignController(int32_t inputDevice);
bool IsControllerConnected(int32_t index);

void RefreshControllerStates();

void ShowSoftKeyboard();
void HideSoftKeyboard();
void InitializeSoftKeyboard();
bool IsSoftKeyboardEnabled();
void RenderSoftKeyboard();
void UpdateSoftKeyboard();

bool IsPointerDownRaw(int32_t pointer = 0);
bool IsTouchDownRaw(int32_t touch);
int32_t CharToKey(char target);

int32_t GetScrollWheelDelta();
void SetScrollWheelDelta(int32_t delta);
