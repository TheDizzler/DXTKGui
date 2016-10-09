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

#include <sstream>
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
}

//const Vector2 MouseController::getMovement() const {
	/*wostringstream ws;
	ws << "MoveBy: " << raw->lLastX << ", " << raw->lLastY << "\n";
	OutputDebugString(ws.str().c_str());
	return Vector2(raw->lLastX, raw->lLastY);*/
//}


int MouseController::scrollWheelValue() {

	int delta = state.scrollWheelValue / WHEEL_DELTA;
	mouse->ResetScrollWheelValue();
	return delta;
}

bool MouseController::leftButton() {
	return state.leftButton;
}

bool MouseController::middleButton() {
	return state.middleButton;
}

bool MouseController::rightButton() {
	return state.rightButton;
}

bool MouseController::leftButtonLast() {
	return lastState.leftButton;
}

bool MouseController::middleButtonLast() {
	return lastState.middleButton;
}

bool MouseController::rightButtonLast() {
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

