#include "MouseController.h"
#include "../GUIFactory.h"
#include "../StringHelper.h"


MouseController::~MouseController() {
}

void MouseController::initialize(HWND h) {
	hwnd = h;
	mouse.SetWindow(hwnd);
	layerDepth = 1.0;
}


void MouseController::setState(Mouse::Mode mode) {
	mouse.SetMode(mode);
}

bool MouseController::loadMouseIcon(GUIFactory* guiFactory,
	const pugi::char_t* spriteName) {

	GraphicsAsset* mouseAsset = guiFactory->getAsset(spriteName);
	if (mouseAsset == NULL) {
		wostringstream ws;
		ws << "Cannot find mouse sprite file: " << spriteName;
		StringHelper::reportError(ws.str().c_str(), L"Critical failure", false);

		return false;
	}

	Sprite::load(mouseAsset);
	setOrigin(mouseAsset->getOrigin());
	mouseAsset = NULL;
	return true;
}

void MouseController::loadMouseIcon(GraphicsAsset* iconAsset) {
	load(iconAsset);
	setOrigin(iconAsset->getOrigin());
	iconAsset = NULL;
}

void MouseController::saveMouseState() {

	lastState = state;
	state = mouse.GetState();
	setPosition(Vector2((float) state.x, (float) state.y));
	// This is the absolute position of the mouse relative
	// to the upper-left corner of the window


	isPressed = !lastState.leftButton && state.leftButton;
	isClicked = lastState.leftButton && !state.leftButton;

	isRightPressed = !lastState.rightButton && state.rightButton;
	isRightClicked = lastState.rightButton && !state.rightButton;

	isMiddlePressed = !lastState.middleButton && state.middleButton;
	isMiddleClicked = lastState.middleButton && !state.middleButton;
}

void MouseController::show() {
	isShown = true;
}

void MouseController::hide() {
	isShown = false;
}

void MouseController::draw(SpriteBatch* batch) {
	if (isShown)
		Sprite::draw(batch);
}

int MouseController::scrollWheelValue() {

	int delta = state.scrollWheelValue / WHEEL_DELTA;
	mouse.ResetScrollWheelValue();
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
		wostringstream wss;
		wss << "Clicked! " << endl;
		OutputDebugString(wss.str().c_str());
		isClicked = false;
		return true;
	}
	return false;
}

bool MouseController::pressed() {

	if (isPressed) {
		wostringstream wss;
		wss << "Pressed! " << endl;
		OutputDebugString(wss.str().c_str());
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

void MouseController::resetPressed() {
	isPressed = true;
	lastState.leftButton = true;
}

