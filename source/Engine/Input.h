#include "../pch.h"
#pragma once


#include "MouseController.h"
#include "KeyboardController.h"

using namespace std;
using namespace DirectX;


class Input {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);
	/* Depracting. */
	void setRawInput(RAWINPUT* raw);

	//void processMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	std::unique_ptr<KeyboardController> keys;
	std::unique_ptr<MouseController> mouse;
	

	//RAWKEYBOARD* rawKeys;
	//RAWMOUSE* rawMouse;
};

