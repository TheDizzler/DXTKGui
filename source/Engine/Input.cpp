#include "Input.h"


Input::Input() {
}


Input::~Input() {

	ReleaseCapture();
	ShowCursor(true);
}

#include "../Managers/GameManager.h"
bool Input::initRawInput(HWND hwnd) {

	//RAWINPUTDEVICE rid[2];
	//rid[0].usUsagePage = 0x01;
	//rid[0].usUsage = 0x06; // keyboard
	//rid[0].dwFlags = RIDEV_NOLEGACY; // do not generate legacy messages such as WM_KEYDOWN
	//rid[0].hwndTarget = hwnd; // 0??

	//rid[1].usUsagePage = 0x01;
	//rid[1].usUsage = 0x02; // mouse
	//rid[1].dwFlags = 0/*RIDEV_NOLEGACY | RIDEV_CAPTUREMOUSE | RIDEV_INPUTSINK*/;
	//rid[1].hwndTarget = hwnd;

	//if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
	//	return false;

	keys.reset(new KeyboardController());
	//mouse.reset(new MouseController(hwnd));
	mouse = make_unique<MouseController>(hwnd);
	if (GameManager::guiFactory != NULL)
		mouse->loadMouseIcon(GameManager::guiFactory.get(), "Mouse Icon");

	
	return true;
}

void Input::setRawInput(RAWINPUT* raw) {

	//if (raw->header.dwType == RIM_TYPEKEYBOARD) {
	//	//rawKeys = &raw->data.keyboard;
	//	keys->getInput(&raw->data.keyboard);
	//}


	//if (raw->header.dwType == RIM_TYPEMOUSE) {
	//	//rawMouse = &raw->data.mouse;
	//	mouse->getRawInput(&raw->data.mouse);
	//}
}

//void Input::processMouseMessage(UINT message, WPARAM wParam, LPARAM lParam) {
//	mouse->ProcessMessage(message, wParam, lParam);
//}


