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
	void getLastRawInput();


	bool leftButtonDown();
	bool midButtonDown();
	bool rightButtonDown();

	bool leftButtonLastDown();
	bool midButtonLastDown();
	bool rightButtonLastDown();

private:

	HWND hwnd;

};