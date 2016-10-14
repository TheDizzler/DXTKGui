#include "KeyboardController.h"

KeyboardController::KeyboardController() {
	keyboard = std::make_unique<Keyboard>();

}

KeyboardController::~KeyboardController() {
}

void KeyboardController::saveKeyboardState() {

	lastState = state;
	state = keyboard->GetState();
}


//void KeyboardController::getInput(RAWKEYBOARD* rawKey) {
//
//	getLastInput();
//
//	bool down;
//	if (rawKey->Message == WM_KEYDOWN) {
//		down = true;
//	} else if (rawKey->Message == WM_KEYUP) {
//		down = false;
//	} else {
//		// something else that we don't care about??
//		return;
//	}
//	//bool otherKeyUp = rawKey->Message == WM_KEYUP;
//	//bool isKeyDown = rawKey->Message == WM_KEYDOWN;
//	//bool keyUp = rawKey->Flags & RI_KEY_BREAK;
//	//bool keyMake = rawKey->Flags & RI_KEY_MAKE; // this is always 0 
//	USHORT keyChar = rawKey->VKey;
//
//	switch (keyChar) {
//		/*case VK_LEFT:
//		case VK_NUMPAD4:*/
//		case 'A':
//		{
//
//			//keyDown[LEFT] = setIsDown(keyDown[LEFT], keyUp);
//			keyDown[LEFT] = down;
//			//std::wostringstream ws;
//			//ws << "keyUp: " << keyUp << "\n";
//			//ws << "WM_KEYDOWN: " << (rawKey->Message == WM_KEYDOWN) << "\n";
//			//ws << "WM_KEYUP: " << (rawKey->Message == WM_KEYUP) << "\n";
//			//ws << "otherKeyUp: " << otherKeyUp << "\n";
//			//ws << "otherKeyDown: " << otherKeyDown << "\n\n";
//			//OutputDebugString(ws.str().c_str());
//		}
//		break;
//
//	/*case VK_RIGHT:
//	case VK_NUMPAD6:*/
//		case 'D':
//			keyDown[RIGHT] = down;
//			break;
//
//		/*case VK_UP:
//		case VK_NUMPAD8:*/
//		case 'W':
//			keyDown[UP] = down;
//			break;
//
//		/*case VK_DOWN:
//		case VK_NUMPAD2:*/
//		case 'S':
//			keyDown[DOWN] = down;
//			break;
//
//		case VK_SPACE:
//			keyDown[FIRE] = down;
//			break;
//
//		case VK_RETURN:
//		case 'C':
//			keyDown[SELECT] = down;
//			break;
//
//		case 'X':
//			keyDown[CANCEL] = down;
//			break;
//
//		case VK_ESCAPE:
//		case 'P':
//			keyDown[ESC] = down;
//			break;
//
//	}
//}


//void KeyboardController::getLastInput() {
//
//	for (int i = 0; i < KEYCOUNT; ++i)
//		lastDown[i] = keyDown[i];
//}
