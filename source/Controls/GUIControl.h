#pragma once

#include <vector>
#include "../Engine/MouseController.h"
#include "../BaseGraphics/FontSet.h"
using namespace std;

//class GUIControlBox;
class GUIControl {
public:
	enum ClickAction {
		EXIT, PLAY, SETTINGS, CANCEL, OK, UP, DOWN

	};

	virtual void update(double deltaTime, MouseController* mouse) = 0;
	virtual void draw(SpriteBatch* batch) = 0;

	virtual void setFont(shared_ptr<FontSet> newFont) = 0;
	virtual void setPosition(Vector2& pos);

	virtual const Vector2& getPosition() = 0;

	virtual void setScale(const Vector2& scl);

	virtual int getWidth() = 0;
	virtual int getHeight() = 0;


	virtual bool clicked() = 0;
	virtual bool selected() = 0;
	virtual bool hovering() = 0;

	ClickAction action;


protected:
	unique_ptr<HitArea> hitArea;
	Vector2 position;
	Vector2 scale = Vector2(1, 1);

	bool isHover = false;
	/** Button is held down over control but has not been released. */
	bool isSelected = false;
	/** While still hovering over control, button has been pressed and released. */
	bool isClicked = false;

};


//interface GUIControlBox : public GUIControl {
//public:
//	void addItems(GUIControl* control);
//	void addItems(std::vector<GUIControl*> controls);
//
//};