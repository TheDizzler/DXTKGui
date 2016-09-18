#include "../pch.h"
#pragma once

#include <d3d11.h>



/** This must be updated when KEYS enum is changed! */
const int KEYCOUNT = 8;


class KeyboardController {
public:

	const enum KEYS {
		DOWN, LEFT, UP, RIGHT,
		SELECT, FIRE, CANCEL, ESC, PAUSE = 7
	};

	KeyboardController();
	~KeyboardController();


	void getInput(RAWKEYBOARD* rawKeys);

	bool keyDown[KEYCOUNT] = {false, false, false, false, false, false, false, false};
	bool lastDown[KEYCOUNT] = {false, false, false, false, false, false, false, false};
private:

	RAWKEYBOARD* rawKey;

	void getLastInput();

};