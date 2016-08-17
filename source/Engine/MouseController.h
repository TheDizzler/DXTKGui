#pragma once

//#include <dinput.h>
#include "../BaseGraphics/Sprite.h"

class MouseButton {
public:
	bool leftButtonDown = false;
	bool midButtonDown = false;
	bool rightButtonDown = false;

};


class MouseController : public Sprite {
public:

	MouseButton currentButtons;
	MouseButton lastButtons;

	MouseController(HWND hwnd);
	~MouseController();

	bool loadMenuMouse();

	/*void loadTexture(ComPtr<ID3D11ShaderResourceView> texture,
		ComPtr<ID3D11Resource> resource);*/

	/** RawInput should be used for moving cameras, etc.
		If only using mouse as a cursor, use GetCursorPos(). */
	void getRawInput(RAWMOUSE* rawMouse);
	void getLastRawInput();


	bool leftButtonDown();
	bool midButtonDown();
	bool rightButtonDown();

	bool leftButtonLastDown();
	bool midButtonLastDown();
	bool rightButtonLastDown();


	void leftButtonHandled();

private:

	HWND hwnd;

};