#pragma once

#include <pugixml.hpp>
#include "Mouse.h"
#include "../BaseGraphics/Sprite.h"



//class MouseButton {
//public:
//	bool leftButton = false;
//	bool middleButton = false;
//	bool rightButton = false;
//
//};

class GUIFactory;
/** A mouse controller that gets rawinput from WM_INPUT in the windows
		message pump. Because the message pump doesn't send mouse rawinput
		when there is no mouse action, the current and last button states
		will freeze in that state. */
class MouseController : public Sprite {
public:

	//MouseButton currentButtons;
	//MouseButton lastButtons;


	MouseController(HWND hwnd);
	~MouseController();

	/* MODE_ABSOLUTE (default) or MODE_RELATIVE (cannot handle relative mode yet). */
	void setState(Mouse::Mode mode);

	/* Could just pass the returned GraphicsAsset pointer but that would
	mean dealing with the error handling every time that the mouse
	icon changes. Alternatively, could load all needed sprites before hand. */
	bool loadMouseIcon(GUIFactory* guiFactory, const  pugi::char_t* spriteName);

	void saveMouseState();

	/*void loadTexture(ComPtr<ID3D11ShaderResourceView> texture,
		ComPtr<ID3D11Resource> resource);*/

	/** RawInput should be used for moving cameras, etc.
		If only using mouse as a cursor, use GetCursorPos(). */
	//void getRawInput(RAWMOUSE* rawMouse);

	/* Dangerous! If no mouse action is sent to windows message pump
		(WM_INPUT) then the same RAWMOUSE info gets recycled. */
	//const Vector2 getMovement() const;

	int scrollWheelValue();
	bool leftButton();
	bool middleButton();
	bool rightButton();

	bool leftButtonLast();
	bool middleButtonLast();
	bool rightButtonLast();

	bool clicked();
	bool pressed();

private:

	HWND hwnd;
	unique_ptr<Mouse> mouse;

	Mouse::State state;
	Mouse::State lastState;
	//RAWMOUSE* raw;

	//void saveLastRawInput();

	short mouseWheelDelta = 0;

	bool isClicked = false;
	bool isPressed = false;
};