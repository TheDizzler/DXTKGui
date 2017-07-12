#pragma once
#include <Windows.h>
#include <pugixml.hpp>

#include <Mouse.h>
#include "../BaseGraphics/Sprite.h"



class GUIFactory;
/** A mouse controller that gets rawinput from WM_INPUT in the windows
		message pump. */
class MouseController : public Sprite {
public:

	MouseController(HWND hwnd);
	virtual ~MouseController();

	/* MODE_ABSOLUTE (default) or MODE_RELATIVE (cannot handle relative mode yet). */
	void setState(Mouse::Mode mode);

	/* Could just pass the returned GraphicsAsset pointer but that would
	mean dealing with the error handling every time that the mouse
	icon changes. Alternatively, could load all needed sprites before hand. */
	bool loadMouseIcon(GUIFactory* guiFactory, const  pugi::char_t* spriteName);
	void loadMouseIcon(GraphicsAsset* iconAsset);

	void saveMouseState();

	void show();
	void hide();
	virtual void draw(SpriteBatch* batch) override;

	int scrollWheelValue();
	bool leftButton();
	bool middleButton();
	bool rightButton();

	bool leftButtonLast();
	bool middleButtonLast();
	bool rightButtonLast();

	bool clicked();
	bool pressed();

	bool rightClicked();
	bool rightPressed();

	bool middleClicked();
	bool middlePressed();

private:

	HWND hwnd;
	unique_ptr<Mouse> mouse;

	Mouse::State state;
	Mouse::State lastState;

	bool isShown = true;

	short mouseWheelDelta = 0;

	bool isClicked = false;
	bool isPressed = false;
	bool isRightClicked = false;
	bool isRightPressed = false;
	bool isMiddleClicked = false;
	bool isMiddlePressed = false;
};