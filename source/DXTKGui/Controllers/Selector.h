#pragma once
#include "..\BaseGraphics\PrimitiveShapes.h"
#include "..\Controls\GUIControl.h"
#include "Joystick.h"


class GUIFactory;
class KeyboardController;

interface Selector {
public:

	virtual ~Selector();

	virtual void reloadGraphicsAsset() = 0;

	virtual void update(double deltaTime) = 0;
	virtual void draw(SpriteBatch* batch) = 0;

	virtual void setDimensions(const Vector2& position, const Vector2& size) = 0;
	virtual void moveBy(const Vector2& moveVector) = 0;
};


class ColorFlashSelector : public Selector {
public:
	ColorFlashSelector(GUIFactory* guiFactory);
	virtual ~ColorFlashSelector();

	virtual void reloadGraphicsAsset() override;


	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void setDimensions(const Vector2& position, const Vector2& size) override;
	virtual void moveBy(const Vector2& moveVector) override;

private:
	unique_ptr<RectangleFrame> frame;

	short frameThickness = 5;

	const double FLASH_TIME = .55;
	double currentFlashTime = 0;
	Color startColor = Colors::Red;
	Color endColor = Colors::White;
	Color color1, color2;

};

/* A manager for simultaneous mouse, joystick and keyboard control.
	NOTE: SelectorManager manages (handles update and draw) of all
	GUIControls added to it.*/
class SelectorManager {
public:
	SelectorManager();
	virtual ~SelectorManager();

	void reloadGraphicsAssets();
	void initialize(unique_ptr<Selector> newSelector);
	void setControllers(Joystick* joy, KeyboardController* keys);
	void setJoystick(Joystick* joystick);

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	bool hasController();

	void addControl(Selectable* control);
	/* Input vector is NOT cleared after. */
	void addControls(vector<Selectable*> controls);

	virtual void moveBy(const Vector2& moveVector);

	void controllerRemoved(ControllerSocketNumber controllerSocket, Joystick* joy);


private:
	unique_ptr<Selector> selector;

	Joystick* joystick = NULL;
	//MouseController* mouse = NULL;
	KeyboardController* keyController = NULL;

	vector<Selectable*> controls;
	SHORT selected = -1;
	void setSelected(SHORT index);

	const double DELAY_TIME = .5;
	double timeSincePressed = DELAY_TIME;


};