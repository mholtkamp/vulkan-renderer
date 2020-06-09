#pragma once

#include <string.h>

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
		nDevice = -1;
		memset(arButtons, 0, VINPUT_CONT_BUTTONS * sizeof(int));
		memset(arAxes, 0, VINPUT_CONT_AXES * sizeof(float));
	}

	int   nDevice;
	int   arButtons[VINPUT_CONT_BUTTONS];
	float arAxes[VINPUT_CONT_AXES];
};

//## **********************************************************************
//## SetKey
//## 
//## Registers that a key is currently being pressed.
//## Should only be called internally by the engine.
//##
//## Input:
//##   nKey - keycode to set.
//## **********************************************************************
void SetKey(int nKey);

//## **********************************************************************
//## ClearKey
//##
//## Indicate that a key is no longer being pressed.
//## Should only be called internally by the engine.
//## 
//## Input:
//##   nKey - keycode to clear.
//## **********************************************************************
void ClearKey(int nKey);

void ResetJusts();

void ClearAllKeys();

//## **********************************************************************
//## IsKeyDown
//##
//## Used to tell if a key is currently being pressed.
//##
//## Input:
//##   nKey - keycode to check.
//##
//## Returns:
//##   int - '1' if key is down.
//##         '0' if key is up.
//## **********************************************************************
int IsKeyDown(int nKey);

int IsKeyJustDownRepeat(int nKey);

int IsKeyJustDown(int nKey);

int IsKeyJustUp(int nKey);

//## **********************************************************************
//## SetButton
//##
//## Registers that a mouse button is pressed down.
//## Should only be called internally by the engine.
//## 
//## Input:
//##   nButton - button code to register.
//## **********************************************************************
void SetButton(int nButton);

//## **********************************************************************
//## ClearButton
//## 
//## Indicate that a button is not being pressed.
//## Should only be called internally by the engine.
//## 
//## Input:
//##   nButton - button code to register.
//## **********************************************************************
void ClearButton(int nButton);

//## **********************************************************************
//## IsButtonDown
//##
//## Used to check if a mouse button is being held down.
//##
//## Input:
//##   nButton - mouse button code to check (see above).
//## 
//## Returns:
//##   int - '1' if mouse button is pressed down.
//##       - '0' if mouse button is not pressed.
//## **********************************************************************
int IsButtonDown(int nButton);

//## **********************************************************************
//## SetTouch
//##
//## Indicate that a touch index is being used (finger touching screen).
//## Should only be used internally by the engine.
//## 
//## Input:
//##   nTouch - touch index.
//## **********************************************************************
void SetTouch(int nTouch);

//## **********************************************************************
//## ClearTouch
//##
//## Indicate that a touch index is not being used.
//## Should only be used internally by the engine.
//##
//## Input:
//##   nTouch - touch index.
//## **********************************************************************
void ClearTouch(int nTouch);

//## **********************************************************************
//## IsTouchDown
//## 
//## Used to check if a touch is down on screen.
//## 
//## Input:
//##   nTouch - touch index to check.
//##
//## Returns:
//##   int - '1' if touch index is down.
//##         '0' otherwise.
//## **********************************************************************
int IsTouchDown(int nTouch);

int IsPointerJustUp(int nPointer = 0);
int IsPointerJustDown(int nPointer = 0);

//## **********************************************************************
//## IsPointerDown
//## 
//## Same as IsTouchDown(), but with the default touch index set to the 
//## mouse index.
//##
//## Input:
//##   nPointer - index of pointer (mouse or touch).
//##
//## Returns:
//##   int - '1' if pointer is pressed down.
//##       - '0' otherwise.
//## **********************************************************************
int IsPointerDown(int nPointer = 0);

//## **********************************************************************
//## GetMousePosition
//## 
//## Used to check the mouse position
//## **********************************************************************
void GetMousePosition(int& nMouseX,
	int& nMouseY);

//## **********************************************************************
//## GetTouchPosition
//## 
//## Gets the position of a pointer in screen coordinates. Coordinate
//## origin is at bottom left corner of the screen.
//##
//## Input:
//##   nTouch - touch index to query.
//##
//## Output:
//##   nTouchX - x coordinate of touch position.
//##   nTouchY - y coordinate of touch position.
//## **********************************************************************
void GetTouchPosition(int& nTouchX,
	int& nTouchY,
	int  nTouch);

//## **********************************************************************
//## GetTouchPositionNormalized
//## 
//## Gets the position of a pointer in normalized screen coordinates. 
//## Coordinate origin is at bottom left corner of the screen.
//##
//## Input:
//##   nTouch - touch index to query.
//##
//## Output:
//##   fTouchX - x coordinate of touch position.
//##   fTouchY - y coordinate of touch position.
//## **********************************************************************
void GetTouchPositionNormalized(float& fTouchX,
	float& fTouchY,
	int    nTouch);

//## **********************************************************************
//## GetPointerPosition
//## 
//## Refer to GetTouchPosition. It is the same as that function except 
//## the touch index is defaulted to 0 (mouse index).
//## **********************************************************************
void GetPointerPosition(int& nPointerX,
	int& nPointerY,
	int  nPointer = 0);

//## **********************************************************************
//## GetPointerPosition
//## 
//## Refer to GetTouchPositionNormalized. It is the same as that function
//##  except the touch index is defaulted to 0 (mouse index).
//## **********************************************************************
void GetPointerPositionNormalized(float& fPointerX,
	float& fPointerY,
	int  nPointer = 0);

//## **********************************************************************
//## SetMousePosition
//## 
//## Register the current mouse location. Should only be called internally
//## by the engine. Origin is at bottom left of screen.
//##
//## Input:
//##   nMouseX - x coordinate of mouse position.
//##   nMouseY - y coordinate of mouse position.
//## **********************************************************************
void SetMousePosition(int nMouseX,
	int nMouseY);

//## **********************************************************************
//## SetTouchPosition
//## 
//## Register the current touch position. Should only be called internally
//## by the engine. Origin as at the bottom left of the screen.
//## 
//## Input:
//##   nTouchX - x coordinate of the touch position.
//##   nTouchY - y cooridnate of the touch position.
//##   nTouch  - touch index.
//## **********************************************************************
void SetTouchPosition(int nTouchX,
	int nTouchY,
	int nTouch = 0);

//## **********************************************************************
//## SetControllerButton
//## 
//## Registers a controller button being pressed down.
//##
//## Input:
//##   nControllerButton - button enum value
//##   nControllerNumber - controller index
//## **********************************************************************
void SetControllerButton(int nControllerButton,
	int nControllerNumber);

//## **********************************************************************
//## ClearControllerButton
//## 
//## Registers a controller button being released.
//##
//## Input:
//##   nControllerButton - button enum value
//##   nControllerNumber - controller index
//## **********************************************************************
void ClearControllerButton(int nControllerButton,
	int nControllerNumber);

//## **********************************************************************
//## IsControllerButtonDown
//## 
//## Queries the state of a controller button.
//##
//## Input:
//##   nControllerButton - button enum value
//##   nControllerNumber - controller index
//##
//## Returns:
//##   int - '1' if button is down.
//##         '0' otherwise.
//## **********************************************************************
int IsControllerButtonDown(int nControllerButton,
	int nControllerNumber);

//## **********************************************************************
//## IsControllerButtonJustDown
//## 
//## Queries if a button was just pressed.
//##
//## Input:
//##   nControllerButton - button enum value
//##   nControllerNumber - controller index
//##
//## Returns:
//##   int - '1' if button is down.
//##         '0' otherwise.
//## **********************************************************************
int IsControllerButtonJustDown(int nControllerButton,
	int nControllerNumber);


//## **********************************************************************
//## SetControllerAxisValue
//## 
//## Sets the value of analog axis.
//##
//## Input:
//##   nControllerAxis   - axis enum value
//##   fAxisValue        - current value of axis.
//##   nControllerNumber - controller index
//## **********************************************************************
void SetControllerAxisValue(int   nControllerAxis,
	float fAxisValue,
	int   nControllerNumber);

//## **********************************************************************
//## GetControllerAxisValue
//##
//## Input:
//##   nControllerAxis   - axis enum value
//##   nControllerNumber - controller index
//##
//## Returns:
//##   float - value of analog axis.
//## **********************************************************************
float GetControllerAxisValue(int nControllerAxis,
	int nControllerNumber);
//## **********************************************************************
//## GetControllerIndex
//##
//## Input:
//##   nInputDevice - device identification number assigned by OS.
//##
//## Returns:
//##   int - controller index
//## **********************************************************************
int GetControllerIndex(int nInputDevice);

//## **********************************************************************
//## AssignController
//##
//## Based on the input device ID, assign a controller to the Vakz engine.
//##
//## Input:
//##   nInputDevice   - axis enum value
//## **********************************************************************
void AssignController(int nInputDevice);

//## **********************************************************************
//## IsControllerConnected
//##
//## Check if a controller has been connected. (Player1, Player2, etc).
//##
//## Input:
//##   nIndex - controller index
//##
//## Returns:
//##   int - '1' if controller has been connected/registered.
//##         '0'
//## **********************************************************************
int IsControllerConnected(int nIndex);

void RefreshControllerStates();

//## **********************************************************************
//## ShowSoftKeyboard
//##
//## Enable the on-screen keyboard for providing keyboard input.
//## **********************************************************************
void ShowSoftKeyboard();

//## **********************************************************************
//## HideSoftKeyboard
//##
//## Disable the on-screen keyboard for providing keyboard input.
//## **********************************************************************
void HideSoftKeyboard();

//## **********************************************************************
//## InitializeSoftKeyboard
//##
//## Used internally to initialize the on-screen keyboard.
//## **********************************************************************
void InitializeSoftKeyboard();

//## **********************************************************************
//## IsSoftKeyboardEnabled
//##
//## Returns:
//##   int - '1' if on-screen keyboard is enabled.
//##         '0' otherwise.
//## **********************************************************************
int IsSoftKeyboardEnabled();

//## **********************************************************************
//## RenderSoftKeyboard
//##
//## Used internally to render the on-screen keyboard to the screen if
//## it has been enabled.
//## **********************************************************************
void RenderSoftKeyboard();

//## **********************************************************************
//## UpdateSoftKeyboard
//##
//## Used internally to check what keys are being pressed on the on-screen
//## keyboard if the on-screen keyboard has been enabled.
//## **********************************************************************
void UpdateSoftKeyboard();

//## **********************************************************************
//## IsPointerDownRaw
//##
//## Same as IsPointerDown, but returns true even if the pointer is down
//## on the on-screen keyboard.
//## **********************************************************************
int IsPointerDownRaw(int nPointer = 0);

//## **********************************************************************
//## IsTouchDownRaw
//##
//## Same as IsTouchDown, but returns true even if the pointer is down
//## on the on-screen keyboard.
//## **********************************************************************
int IsTouchDownRaw(int nTouch);

//## **********************************************************************
//## CharToKey
//##
//## Converts an ascii character to the key that corresponds with it.
//## Example: CharToKey('A') will return VKEY_A.
//##
//## Input:
//##   cTarget - character to convert.
//##
//## Returns:
//##   int - Key enum value
//## **********************************************************************
int CharToKey(char cTarget);

int GetScrollWheelDelta();

void SetScrollWheelDelta(int nDelta);
