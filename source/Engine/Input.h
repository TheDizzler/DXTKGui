#pragma once


#include "../DXTKGui/Controllers/MouseController.h"
#include "../DXTKGui/Controllers/KeyboardController.h"

using namespace DirectX;


class Input {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);
	/* Depracting. */
	//void setRawInput(RAWINPUT* raw);


protected:

	std::unique_ptr<KeyboardController> keys;
	std::unique_ptr<MouseController> mouse;
	

	//RAWKEYBOARD* rawKeys;
	//RAWMOUSE* rawMouse;
};

