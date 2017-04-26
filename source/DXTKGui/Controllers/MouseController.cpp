#include "MouseController.h"

MouseController::MouseController(HWND h) {

	hwnd = h;
	mouse.reset(new Mouse());
	mouse->SetWindow(hwnd);

}


MouseController::~MouseController() {
}

#include "../GUIFactory.h"
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

void MouseController::loadMouseIcon(GraphicsAsset* iconAsset) {
	load(iconAsset);
	setOrigin(iconAsset->getOrigin());
}

//#include "../../Engine/GameEngine.h"
void MouseController::saveMouseState() {

	lastState = state;
	state = mouse->GetState();
	setPosition(Vector2(state.x, state.y));
	// This is the absolute position of the mouse relative
	// to the upper-left corner of the window


	isPressed = !lastState.leftButton && state.leftButton;
	isClicked = lastState.leftButton && !state.leftButton;

	isRightPressed = !lastState.rightButton && state.rightButton;
	isRightClicked = lastState.rightButton && !state.rightButton;

	isMiddlePressed = !lastState.middleButton && state.middleButton;
	isMiddleClicked = lastState.middleButton && !state.middleButton;
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

bool MouseController::rightClicked() {

	if (isRightClicked) {
		isRightClicked = false;
		return true;
	}
	return false;
}

bool MouseController::rightPressed() {
	if (isRightPressed) {
		isRightPressed = false;
		return true;
	}
	return false;
}

bool MouseController::middleClicked() {
	if (isMiddleClicked) {
		isMiddleClicked = false;
		return true;
	}
	return false;
}

bool MouseController::middlePressed() {
	if (isMiddlePressed) {
		isMiddlePressed = false;
		return true;
	}
	return false;
}

