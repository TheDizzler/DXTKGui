#include "MouseController.h"

MouseController::MouseController(HWND hWnd) {

	hwnd = hWnd;
}

MouseController::~MouseController() {
}

void MouseController::getRawInput(RAWMOUSE* raw) {

	getLastRawInput();

	bool bDown = raw->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN;
	bool bUp = raw->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP;
	if (bDown && !bUp)
		currentButtons.leftButtonDown = true;
	else if (bUp)
		currentButtons.leftButtonDown = false;

	bDown = raw->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN;
	bUp = raw->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP;
	if (bDown && !bUp)
		currentButtons.midButtonDown = true;
	else if (bUp)
		currentButtons.midButtonDown = false;

	bDown = raw->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN;
	bUp = raw->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP;
	if (bDown && !bUp)
		currentButtons.midButtonDown = true;
	else if (bUp)
		currentButtons.midButtonDown = false;

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hwnd, &cursorPos);
	//SetCursorPos(cursorPos.x, cursorPos.y);
	setPosition(Vector2(cursorPos.x, cursorPos.y + 30));

}

void MouseController::getLastRawInput() {

	lastButtons = currentButtons;
}

//DIMOUSESTATE MouseController::setCurrentState() {
//
//	lastState = currentState;
//
//	lastButtons = currentButtons;
//	/*lastButtons.leftButtonDown = currentButtons.leftButtonDown;
//	lastButtons.midButtonDown = currentButtons.midButtonDown;
//	lastButtons.rightButtonDown = currentButtons.rightButtonDown;*/
//
//
//	currentButtons.leftButtonDown = GetKeyState(VK_LBUTTON) & 0x8000;
//	currentButtons.midButtonDown = GetKeyState(VK_MBUTTON) & 0x8000;
//	currentButtons.rightButtonDown = GetKeyState(VK_RBUTTON) & 0x8000;
//
//	return currentState;
//}

bool MouseController::leftButtonDown() {
	return currentButtons.leftButtonDown;
}

bool MouseController::midButtonDown() {
	return currentButtons.midButtonDown;
}

bool MouseController::rightButtonDown() {
	return currentButtons.rightButtonDown;
}

bool MouseController::leftButtonLastDown() {
	return lastButtons.leftButtonDown;
}

bool MouseController::midButtonLastDown() {
	return lastButtons.midButtonDown;
}

bool MouseController::rightButtonLastDown() {
	return lastButtons.rightButtonDown;
}

void MouseController::leftButtonHandled() {

	currentButtons.leftButtonDown = false;
}

