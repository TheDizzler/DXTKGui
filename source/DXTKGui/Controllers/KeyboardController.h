#pragma once

#include <Windows.h>
#include "Keyboard.h"

using namespace DirectX;

/** This must be updated when KEYS enum is changed! */
//const int KEYCOUNT = 8;

/** This class seems superfluous now. */
class KeyboardController {
public:

	/*const enum KEYS {
		DOWN, LEFT, UP, RIGHT,
		SELECT, FIRE, CANCEL, ESC, PAUSE = 7
	};*/

	KeyboardController();
	~KeyboardController();

	void saveKeyboardState();

	//void getInput(RAWKEYBOARD* rawKeys);

	/*bool keyDown[KEYCOUNT] = {false, false, false, false, false, false, false, false};
	bool lastDown[KEYCOUNT] = {false, false, false, false, false, false, false, false};*/
private:

	std::unique_ptr<Keyboard> keyboard;

	Keyboard::State state;
	Keyboard::State lastState;

	//RAWKEYBOARD* rawKey;

	//void getLastInput();

};