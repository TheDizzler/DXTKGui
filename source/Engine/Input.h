#pragma once


#include "MouseController.h"
#include "KeyboardController.h"


using namespace DirectX;


class Input {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);

	void setRawInput(RAWINPUT* raw);



protected:

	std::unique_ptr<KeyboardController> keys;
	std::unique_ptr<MouseController> mouse;

	RAWKEYBOARD* rawKeys;
	RAWMOUSE* rawMouse;
};

