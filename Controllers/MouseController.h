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

	virtual ~MouseController();

	void initialize(HWND hwnd);

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

	/** After release of button. */
	bool clicked();
	/** On button down. */
	bool pressed();

	/** After release of button. */
	bool rightClicked();
	/** On button down. */
	bool rightPressed();

	/** After release of button. */
	bool middleClicked();
	/** On button down. */
	bool middlePressed();
	/* Test to fix dissappearing button clicks after full screen change.
		Seems none functional. */
	void resetPressed();

private:

	HWND hwnd;
	Mouse mouse;

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

/* From SquareWarz. May be useful. */
interface MouseSelectableObject {
public:
	virtual bool contains(const Vector2& point) = 0;
	virtual void mouseListener(shared_ptr<MouseController> mouse) = 0;

	virtual const float getLayerDepth() const = 0;

	/** Action to perform when mouse button released over control. */
	virtual void onClick() = 0;
	/** Action to perform when mouse button held over control. */
	virtual void onPress() = 0;
	/** Action to perform when mouse over control. */
	virtual void onHover() = 0;
	/** Action to perform to return control back to neutral state. */
	virtual void resetState() = 0;

	virtual bool clicked() = 0;
	/* Is Mouse Button down over control? */
	virtual bool pressed() = 0;
	virtual bool hovering() = 0;
};