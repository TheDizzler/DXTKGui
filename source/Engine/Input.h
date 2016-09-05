#pragma once

//#pragma comment (lib, "dinput8.lib")

//#include <dinput.h>
//#include <DirectXMath.h>
//#include <GamePad.h>

#include "MouseController.h"
#include "KeyboardController.h"


using namespace DirectX;


class Input {
public:
	Input();
	~Input();

	bool initRawInput(HWND hwnd);

	void setRawInput(RAWINPUT* raw);



protected:

	std::unique_ptr<KeyboardController> keys;
	std::unique_ptr<MouseController> mouse;

	RAWKEYBOARD* rawKeys;
	RAWMOUSE* rawMouse;
};

