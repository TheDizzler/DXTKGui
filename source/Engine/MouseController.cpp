#include "MouseController.h"

MouseController::MouseController(HWND hWnd) {

	hwnd = hWnd;
	mouse = make_unique<Mouse>();
	mouse->SetWindow(hwnd);
}


MouseController::~MouseController() {
}

#include "../Controls/GUIFactory.h"
void MouseController::setState(Mouse::Mode mode) {
	mouse->SetMode(mode);
}


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


void MouseController::saveMouseState() {

	lastState = state;
	state = mouse->GetState();
	setPosition(Vector2(state.x, state.y));
	// This is the absolute position of the mouse relative
	// to the upper-left corner of the window


	isPressed = !lastState.leftButton && state.leftButton;
	isClicked = lastState.leftButton && !state.leftButton;



	//int i = 0;
	//void MouseController::getRawInput(RAWMOUSE* rawMouse) {
	//
	//	raw = rawMouse;
	//	saveLastRawInput();
	//
	//	const USHORT buttonFlags = raw->usButtonFlags;
	//
	//	if (buttonFlags & RI_MOUSE_WHEEL) {
	//		mouseWheelDelta = (short) raw->usButtonData;
	//		/*wostringstream ws;
	//		ws << "mouseWheel: " << mouseWheel << "\n";
	//		OutputDebugString(ws.str().c_str());*/
	//	}
	//
	//
	//	if (buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
	//		currentButtons.leftButton = true;
	//	} else if (buttonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
	//		currentButtons.leftButton = false;
	//	}
	//
	//	if (buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
	//		currentButtons.middleButton = true;
	//	else if (buttonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
	//		currentButtons.middleButton = false;
	//
	//	if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
	//		currentButtons.rightButton = true;
	//	else if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
	//		currentButtons.rightButton = false;
	//
	//	POINT cursorPos;
	//	GetCursorPos(&cursorPos);
	//	ScreenToClient(hwnd, &cursorPos);
	//	setPosition(Vector2(cursorPos.x, cursorPos.y));
	//
	//	isPressed = !lastButtons.leftButton && currentButtons.leftButton;
	//
	//	if (lastButtons.leftButton && !currentButtons.leftButton) {
	//		isClicked = true;
	//	} else {
	//		isClicked = false;
	//
	//	}
}

//const Vector2 MouseController::getMovement() const {
	/*wostringstream ws;
	ws << "MoveBy: " << raw->lLastX << ", " << raw->lLastY << "\n";
	OutputDebugString(ws.str().c_str());
	return Vector2(raw->lLastX, raw->lLastY);*/
//}


//void MouseController::saveLastRawInput() {
//
//	lastButtons = currentButtons;
//
//}

int MouseController::scrollWheelValue() {

	int delta = state.scrollWheelValue / WHEEL_DELTA;
	mouse->ResetScrollWheelValue();
	return delta;
	/*short delta = mouseWheelDelta / WHEEL_DELTA;
	mouseWheelDelta = 0;
	return delta;*/
}

bool MouseController::leftButton() {

	//return currentButtons.leftButton;
	return state.leftButton;
}

bool MouseController::middleButton() {
	//return currentButtons.middleButton;
	return state.middleButton;
}

bool MouseController::rightButton() {
	//return currentButtons.rightButton;
	return state.rightButton;
}

bool MouseController::leftButtonLast() {
	//return lastButtons.leftButton;
	return lastState.leftButton;
}

bool MouseController::middleButtonLast() {
	//return lastButtons.middleButton;
	return lastState.middleButton;
}

bool MouseController::rightButtonLast() {
	//return lastButtons.rightButton;
	return lastState.rightButton;
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

