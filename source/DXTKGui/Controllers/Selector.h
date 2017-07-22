#pragma once
#include "..\BaseGraphics\PrimitiveShapes.h"
#include "..\Controls\GUIControl.h"
#include "Joystick.h"

class GUIFactory;

class Selector {
public:
	Selector();
	virtual ~Selector();

	void setJoystick(Joystick* joystick);
	bool initialize(GUIFactory* guiFactory);

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	bool hasController();
	bool isControllerSocket(ControllerSocketNumber socketNumber);

	void addControl(GUIControl* control);
	/* vector is NOT cleared after. */
	void addControls(vector<GUIControl*> controls);

private:
	unique_ptr<RectangleFrame> frame;
	Joystick* joystick = NULL;
	vector<GUIControl*> controls;
	SHORT selected = -1;
	void setSelected(SHORT index);

	short frameThickness = 5;

	const double DELAY_TIME = .5;
	double timeSincePressed = DELAY_TIME;

};