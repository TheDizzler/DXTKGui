#pragma once

#include <Windows.h>
#include "Keyboard.h"

using namespace DirectX;

/** This class seems superfluous now. This class may become obsolete. */
class KeyboardController {
public:


	KeyboardController();
	~KeyboardController();

	void saveKeyboardState();

private:

	std::unique_ptr<Keyboard> keyboard;

	Keyboard::State state;
	Keyboard::State lastState;

};