#pragma once
#include "..\BaseGraphics\PrimitiveShapes.h"
#include "..\Controls\GUIControl.h"
#include "Joystick.h"
#include <Keyboard.h>

class GUIFactory;

/* A simple GUIControl selector for joystick control. If no mouse support needed,
	utilize this.*/
class Selector {
public:
	Selector();
	virtual ~Selector();

	void reloadGraphicsAsset();
	void setJoystick(Joystick* joystick);
	bool initialize(GUIFactory* guiFactory);

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	bool hasController();
	bool isControllerSocket(ControllerSocketNumber socketNumber);

	void addControl(GUIControl* control);
	/* Input vector is NOT cleared after. */
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

/* A manager for simultaneous mouse, joystick and keyboard control.
	NOTE: SelectorManager manages (handles update and draw) of all
	GUIControls added to it.*/
class SelectorManager {
public:
	SelectorManager();
	virtual ~SelectorManager();

	void reloadGraphicsAssets();
	bool initialize(GUIFactory* guiFactory);
	void setControllers(shared_ptr<MouseController> mouse, Joystick* joy);
	void setJoystick(Joystick* joystick);

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	bool hasController();

	void addControl(Selectable* control);
	/* Input vector is NOT cleared after. */
	void addControls(vector<Selectable*> controls);

	void controllerRemoved(ControllerSocketNumber controllerSocket, Joystick* joy);


private:
	unique_ptr<RectangleFrame> frame;

	Joystick* joystick = NULL;
	shared_ptr<MouseController> mouse;
	Keyboard::KeyboardStateTracker keyTracker;

	vector<Selectable*> controls;
	SHORT selected = -1;
	void setSelected(SHORT index);

	short frameThickness = 5;

	const double DELAY_TIME = .5;
	double timeSincePressed = DELAY_TIME;
};