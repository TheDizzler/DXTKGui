#pragma once
#include <Windows.h>
#include <Keyboard.h>

using namespace DirectX;

class KeyboardController {
public:
	//KeyboardController();
	virtual ~KeyboardController();

	/** Call this once at the beginning of the frame. */
	void saveKeyState();

	bool isKeyPressed(Keyboard::Keys key);
	bool isKeyReleased(Keyboard::Keys key);

public:
	Keyboard keyboard;
	Keyboard::State keyState;
	Keyboard::KeyboardStateTracker keyTracker;

};