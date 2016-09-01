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

int i = 0;
void MouseController::getRawInput(RAWMOUSE* raw) {

	saveLastRawInput();

	bool bDown = raw->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN;
	bool bUp = raw->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP;
	if (bDown && !bUp) {
		currentButtons.leftButtonDown = true;
		/*wostringstream ws;
		ws << "Down!\n";
		OutputDebugString(ws.str().c_str());*/
	} else if (bUp) {
		currentButtons.leftButtonDown = false;
		/*wostringstream ws;
		ws << "Up!\n";
		OutputDebugString(ws.str().c_str());*/
	}

	bDown = raw->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN;
	bUp = raw->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP;
	if (bDown && !bUp)
		currentButtons.midButtonDown = true;
	else if (bUp)
		currentButtons.midButtonDown = false;

	bDown = raw->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN;
	bUp = raw->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP;
	if (bDown && !bUp)
		currentButtons.rightButtonDown = true;
	else if (bUp)
		currentButtons.rightButtonDown = false;

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hwnd, &cursorPos);
	setPosition(Vector2(cursorPos.x, cursorPos.y));

	isPressed = !lastButtons.leftButtonDown && currentButtons.leftButtonDown;

	if (lastButtons.leftButtonDown && !currentButtons.leftButtonDown) {
		/*wostringstream ws;
		ws << "Clicked! " << ++i << "\n";
		OutputDebugString(ws.str().c_str());*/
		isClicked = true;
	} else {
		isClicked = false;

	}
}


void MouseController::saveLastRawInput() {

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


bool MouseController::clicked() {

	if (isClicked) {
		isClicked = false;
		return true;
	}
	return false;
}

bool MouseController::pressed() {

	if (isPressed) {
		isPressed = false;
		return true;
	}
	return false;
}

