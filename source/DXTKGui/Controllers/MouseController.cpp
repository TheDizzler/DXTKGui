#include "MouseController.h"

MouseController::MouseController(HWND hWnd) {

	hwnd = hWnd;
	mouse.reset(new Mouse());
	mouse->SetWindow(hwnd);

}

//MouseController::MouseController(HWND hWnd, Mouse& mse) {
//
//	hwnd = hWnd;
//
//	mouse.reset(&mse);
//	hitArea.reset(new HitArea(
//		Vector2(position.x/* - origin.x*/, position.y/* - origin.y*/),
//		Vector2(width, height)));
//}


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
	setOrigin(mouseAsset->getOrigin());

	return true;
}

#include "../../Engine/GameEngine.h"
void MouseController::saveMouseState() {

	lastState = state;
	state = mouse->GetState();
	setPosition(Vector2(state.x, state.y));
	// This is the absolute position of the mouse relative
	// to the upper-left corner of the window


	isPressed = !lastState.leftButton && state.leftButton;
	isClicked = lastState.leftButton && !state.leftButton;

}


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

