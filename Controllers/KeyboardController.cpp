#include "KeyboardController.h"


KeyboardController::~KeyboardController() {
}

void KeyboardController::saveKeyState() {

	keyState = keyboard.GetState();
	keyTracker.Update(keyState);

}

bool KeyboardController::isKeyPressed(Keyboard::Keys key) {
	return keyTracker.IsKeyPressed(key);
}

bool KeyboardController::isKeyReleased(Keyboard::Keys key) {
	return keyTracker.IsKeyReleased(key);
}
