#include "Selector.h"
#include "..\GUIFactory.h"


Selector::Selector() {
}

Selector::~Selector() {
	controls.clear();
}

bool Selector::initialize(GUIFactory* guiFactory) {

	frame.reset(guiFactory->createRectangleFrame());
	frame->setTint(Colors::Red);
	return true;
}

void Selector::setJoystick(Joystick* joy) {
	joystick = joy;
}


void Selector::update(double deltaTime) {

	if (joystick) {
		if (joystick->aButtonPushed()) {
			controls[selected]->onClick();
			timeSincePressed = DELAY_TIME;
		} else if (joystick->isUpPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(--selected);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else if (joystick->isDownPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(++selected);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else
			timeSincePressed = DELAY_TIME;
	}
	frame->update();
}

void Selector::draw(SpriteBatch* batch) {
	if (selected > -1)
		frame->draw(batch);
}

bool Selector::hasController() {
	return joystick;
}

bool Selector::isControllerSocket(ControllerSocketNumber socketNumber) {
	return joystick->getControllerSockerNumber() == socketNumber;
}

void Selector::addControl(GUIControl* control) {
	controls.push_back(control);

	if (selected == -1) {
		setSelected(0);
	}
}

void Selector::addControls(vector<GUIControl*> controls) {
	for (const auto& control : controls)
		controls.push_back(control);
}

void Selector::setSelected(SHORT index) {
	if (index < 0)
		selected = controls.size() - 1;
	else if (index > controls.size() - 1)
		selected = 0;
	else
		selected = index;

	frame->setDimensions(
		controls[selected]->getPosition(),
		Vector2(controls[selected]->getWidth(), controls[selected]->getHeight()),
		frameThickness);
}
