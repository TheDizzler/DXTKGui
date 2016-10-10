#include "Input.h"


Input::Input() {
}


Input::~Input() {

	ReleaseCapture();
	ShowCursor(true);
}

#include "../Managers/GameManager.h"
bool Input::initRawInput(HWND hwnd) {

	keys.reset(new KeyboardController());
	mouse = make_unique<MouseController>(hwnd);
	if (GameManager::guiFactory != NULL)
		mouse->loadMouseIcon(GameManager::guiFactory.get(), "Mouse Icon");

	
	return true;
}

//void Input::setRawInput(RAWINPUT* raw) {

	//if (raw->header.dwType == RIM_TYPEKEYBOARD) {
	//	//rawKeys = &raw->data.keyboard;
	//	keys->getInput(&raw->data.keyboard);
	//}


	//if (raw->header.dwType == RIM_TYPEMOUSE) {
	//	//rawMouse = &raw->data.mouse;
	//	mouse->getRawInput(&raw->data.mouse);
	//}
//}


