#pragma once

#include "../DXTKGui/GUIFactory.h"
#include "../Engine/Joystick.h"

#include "../globals.h"


class GUIOverlay {
public:
	GUIOverlay();
	virtual ~GUIOverlay();


	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	void setDialogText(USHORT playerSlotNumber, wstring text);

	void reportLostJoystick(size_t playerSlotNumber);


private:

	unique_ptr<TextLabel> fpsLabel;

	vector<unique_ptr<Dialog>> lostJoyDialogs;
	vector<int> displayingLostJoys;


};


class ControllerDialog : public PromptDialog {
public:
	ControllerDialog(GUIFactory* guiFactory);

	virtual void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 10);

	virtual void update(double deltaTime) override;


	virtual void setText(wstring text) override;

private:
	wstring defaultText;
	double dialogOpenTime = 0;
	const double CONTROLLER_WAIT_TIME = 1.0;
	int ellipsisii = 16;
	bool first = true;
};