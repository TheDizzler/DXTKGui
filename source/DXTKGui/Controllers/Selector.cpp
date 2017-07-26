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

void Selector::reloadGraphicsAsset() {
	frame->reloadGraphicsAsset();
}

void Selector::setJoystick(Joystick* joy) {
	joystick = joy;
}


void Selector::update(double deltaTime) {

	if (controls.size() == 0)
		return;
	if (joystick) {
		if (joystick->aButtonPushed()) {
			controls[selected]->onClick();
			controls[selected]->onHover();
			timeSincePressed = DELAY_TIME;

		} else if (joystick->isUpPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected - 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else if (joystick->isDownPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected + 1);
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

	if (selected == -1) {
		setSelected(0);
	}
}

void Selector::setSelected(SHORT index) {


	if (selected > -1 && selected < controls.size())
		controls[selected]->resetState();

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

	controls[selected]->onHover();
}




SelectorManager::SelectorManager() {
}

SelectorManager::~SelectorManager() {
	joystick = NULL;
}

void SelectorManager::reloadGraphicsAssets() {
	for (const auto& control : controls)
		control->reloadGraphicsAsset();

	frame->reloadGraphicsAsset();
}

bool SelectorManager::initialize(GUIFactory* guiFactory) {
	frame.reset(guiFactory->createRectangleFrame());
	frame->setTint(Colors::Red);
	return true;
}

void SelectorManager::setControllers(shared_ptr<MouseController> ms,
	Joystick* joy) {

	mouse = ms;
	joystick = joy;
}

void SelectorManager::setJoystick(Joystick* joy) {
	joystick = joy;
}

void SelectorManager::update(double deltaTime) {

	

	for (int i = 0; i < controls.size(); ++i) {
		if (controls[i]->update(deltaTime))
			int check = 0;
		if (selected != i && controls[i]->hovering()) {
			setSelected(i);
		}

	}

	if (joystick) {
		if (joystick->aButtonPushed()) {
			controls[selected]->onClick();
			controls[selected]->onHover();
			timeSincePressed = DELAY_TIME;

		} else if (joystick->isUpPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected - 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else if (joystick->isDownPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected + 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else
			timeSincePressed = DELAY_TIME;
	}

	frame->update();
}

void SelectorManager::draw(SpriteBatch* batch) {
	for (const auto& control : controls)
		control->draw(batch);

	if (selected > -1)
		frame->draw(batch);
}

bool SelectorManager::hasController() {
	return joystick;
}

void SelectorManager::addControl(GUIControl* control) {
	controls.push_back(control);

	if (selected == -1) {
		setSelected(0);
	}
}

void SelectorManager::addControls(vector<GUIControl*> controls) {
	for (const auto& control : controls)
		controls.push_back(control);

	if (selected == -1) {
		setSelected(0);
	}
}

void SelectorManager::controllerRemoved(
	ControllerSocketNumber socketNumber, Joystick* joy) {

	if (joystick->getControllerSockerNumber() == socketNumber) {
		joystick = joy;
	}
}

void SelectorManager::setSelected(SHORT index) {

	if (selected > -1 && selected < controls.size())
		controls[selected]->resetState();

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

	controls[selected]->onHover();
}
