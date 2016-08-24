#include "MouseController.h"

MouseController::MouseController(HWND hWnd) {

	hwnd = hWnd;
}


MouseController::~MouseController() {
}

#include "../Controls/GUIFactory.h"
/* Could just pass the returned GraphicsAsset pointer but that would
	mean dealing with the error handling every time. */
bool MouseController::loadMouseIcon(GUIFactory* guiFactory,
	const pugi::char_t* spriteName) {

	GraphicsAsset* mouseAsset = guiFactory->getAsset(spriteName);
	if (mouseAsset == NULL) {
		wostringstream ws;
		ws << "Cannot find mouse sprite file: " << spriteName;
		MessageBox(0, ws.str().c_str(), L"Critical failure", MB_OK);

		return false;
	}

	Sprite::load(mouseAsset);

	return true;
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
	setPosition(Vector2(cursorPos.x, cursorPos.y));

}

void MouseController::getLastRawInput() {

	lastButtons = currentButtons;
}

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

