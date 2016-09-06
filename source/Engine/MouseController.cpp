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
void MouseController::getRawInput(RAWMOUSE* rawMouse) {

	raw = rawMouse;
	/*wostringstream ws;
	ws << "MoveBy: " << raw->lLastX << ", " << raw->lLastY << "\n";
	OutputDebugString(ws.str().c_str());*/
	saveLastRawInput();

	const USHORT buttonFlags = raw->usButtonFlags;

	if (buttonFlags & RI_MOUSE_WHEEL) {
		mouseWheelDelta = (short) raw->usButtonData;
		/*wostringstream ws;
		ws << "mouseWheel: " << mouseWheel << "\n";
		OutputDebugString(ws.str().c_str());*/
	}


	if (buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
		currentButtons.leftButtonDown = true;
	} else if (buttonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
		currentButtons.leftButtonDown = false;
	}

	if (buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		currentButtons.midButtonDown = true;
	else if (buttonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
		currentButtons.midButtonDown = false;

	if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		currentButtons.rightButtonDown = true;
	else if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		currentButtons.rightButtonDown = false;

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hwnd, &cursorPos);
	setPosition(Vector2(cursorPos.x, cursorPos.y));

	isPressed = !lastButtons.leftButtonDown && currentButtons.leftButtonDown;

	if (lastButtons.leftButtonDown && !currentButtons.leftButtonDown) {
		isClicked = true;
	} else {
		isClicked = false;

	}
}

const Vector2 MouseController::getMovement() const {
	wostringstream ws;
	ws << "MoveBy: " << raw->lLastX << ", " << raw->lLastY << "\n";
	OutputDebugString(ws.str().c_str());
	return Vector2(raw->lLastX, raw->lLastY);
}


void MouseController::saveLastRawInput() {

	lastButtons = currentButtons;

}

const short MouseController::getWheelDelta() {

	short delta = mouseWheelDelta / WHEEL_DELTA;
	mouseWheelDelta = 0;
	return delta;
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

