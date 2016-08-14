#include "Input.h"


Input::Input() {
}


Input::~Input() {

	ReleaseCapture();
	ShowCursor(true);
}

bool Input::initRawInput(HWND hwnd) {

	RAWINPUTDEVICE rid[2];
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x06; // keyboard
	rid[0].dwFlags = RIDEV_NOLEGACY; // do not generate legacy messages such as WM_KEYDOWN
	rid[0].hwndTarget = hwnd; // 0??

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x02; // mouse
	rid[1].dwFlags = /*RIDEV_CAPTUREMOUSE|*/ RIDEV_INPUTSINK;
	rid[1].hwndTarget = hwnd;
	
	if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
		return false;

	keys.reset(new KeyboardController());
	mouse.reset(new MouseController(hwnd));
	

	return true;
}

void Input::setRawInput(RAWINPUT* raw) {

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
		keys->getInput(&raw->data.keyboard);

	if (raw->header.dwType == RIM_TYPEMOUSE)
		mouse->getRawInput(&raw->data.mouse);
}


