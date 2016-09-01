#pragma once

#include <pugixml.hpp>
#include "../BaseGraphics/Sprite.h"



class MouseButton {
public:
	bool leftButtonDown = false;
	bool midButtonDown = false;
	bool rightButtonDown = false;

};

class GUIFactory;
/** A mouse controller that gets rawinput from WM_INPUT in the windows
		message pump. Because the message pump doesn't send mouse rawinput
		when there is no mouse action, the current and last button states
		will freeze in that state. */
class MouseController : public Sprite {
public:

	MouseButton currentButtons;
	MouseButton lastButtons;

	MouseController(HWND hwnd);
	~MouseController();

	bool loadMouseIcon(GUIFactory* guiFactory, const  pugi::char_t* spriteName);

	/*void loadTexture(ComPtr<ID3D11ShaderResourceView> texture,
		ComPtr<ID3D11Resource> resource);*/

	/** RawInput should be used for moving cameras, etc.
		If only using mouse as a cursor, use GetCursorPos(). */
	void getRawInput(RAWMOUSE* rawMouse);
	


	bool leftButtonDown();
	bool midButtonDown();
	bool rightButtonDown();

	bool leftButtonLastDown();
	bool midButtonLastDown();
	bool rightButtonLastDown();

	bool clicked();
	bool pressed();

private:

	HWND hwnd;
	void saveLastRawInput();

	bool isClicked = false;
	bool isPressed = false;
};