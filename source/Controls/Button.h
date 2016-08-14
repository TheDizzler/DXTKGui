#pragma once

#include "../Engine/MouseController.h"

using namespace std;



/** A visual and logical representation of a button.
Provides no actions on click; that must be handled else where.*/
class Button {
public:

	enum ButtonAction {
		EXIT, PLAY, SETTINGS, CANCEL, OK, UP, DOWN

	};
	Button();
	~Button();

	virtual void update(double deltaTime, MouseController* mouse);
	virtual void draw(SpriteBatch* batch) = 0;

	virtual void setPosition(Vector2& position);
	const Vector2& getPosition();

	void setScale(const Vector2& scale);
	int getWidth();
	int getHeight();

	Color normalColor = Color((Vector3(1, 1, 1)));
	Color hoverColor = Color((Vector3(.5, .75, 1)));;
	Color selectedColor = Color((Vector3(0, .5, 1)));;

	bool clicked();
	bool selected();
	bool hovering();
	ButtonAction action;

protected:

	//bool lastButtonStateDown;

	Vector2 position;
	/* hitArea is taken from Sprite. */
	const HitArea* hitArea;

	unique_ptr<Sprite> normalSprite;
	unique_ptr<Sprite> pressedSprite;

	/** Mouse is over button but mousebutton has not been pressed. */
	bool isHover = false;
	/** Button is held down over mousebutton but has not been released. */
	bool isSelected = false;
	bool isClicked = false;

};