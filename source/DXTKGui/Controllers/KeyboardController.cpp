#include "KeyboardController.h"

KeyboardController::KeyboardController() {
	keyboard = std::make_unique<Keyboard>();

}

KeyboardController::~KeyboardController() {
}

void KeyboardController::saveKeyboardState() {

	lastState = state;
	state = keyboard->GetState();
}
