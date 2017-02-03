#pragma once


#include "../DXTKGui/Controllers/MouseController.h"
#include "../DXTKGui/Controllers/KeyboardController.h"



class Input {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);


protected:

	unique_ptr<KeyboardController> keys;
	shared_ptr<MouseController> mouse;

};

