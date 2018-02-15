#include "Selector.h"
#include "..\GUIFactory.h"
#include "KeyboardController.h"

Selector::~Selector() {

}


ColorFlashSelector::ColorFlashSelector(GUIFactory* guiFactory) {

	frame.reset(guiFactory->createRectangleFrame());
	frame->setTint(Colors::White, false);
}

ColorFlashSelector::~ColorFlashSelector() {
}

void ColorFlashSelector::reloadGraphicsAsset() {
	frame->reloadGraphicsAsset();
}

void ColorFlashSelector::update(double deltaTime) {

	currentFlashTime += deltaTime;
	frame->setTint(Color::Lerp(color1, color2, float(currentFlashTime / FLASH_TIME)), true);
	if (currentFlashTime > FLASH_TIME) {
		Color temp = color1;
		color1 = color2;
		color2 = temp;
		currentFlashTime = 0;
	}
	frame->update();

}

void ColorFlashSelector::draw(SpriteBatch* batch) {
	frame->draw(batch);
}

void ColorFlashSelector::setDimensions(const Vector2& pos, const Vector2& size) {
	frame->setDimensions(pos, size, frameThickness);
	currentFlashTime = 0;
	color1 = startColor;
	color2 = endColor;
}

void ColorFlashSelector::moveBy(const Vector2& moveVector) {

	frame->moveBy(moveVector);
}



SelectorManager::SelectorManager() {
}

SelectorManager::~SelectorManager() {
	joystick = NULL;
	for (Selectable* control : controls)
		delete control;

	controls.clear();
}

void SelectorManager::reloadGraphicsAssets() {
	for (const auto& control : controls)
		control->reloadGraphicsAsset();

	selector->reloadGraphicsAsset();
}

void SelectorManager::initialize(unique_ptr<Selector> newSelector) {

		/*selector = make_unique<ColorFlashSelector>();
		selector->initialize(guiFactory);*/
	selector = move(newSelector);
}

void SelectorManager::setControllers(Joystick* joy, KeyboardController* key) {

	//mouse = ms;
	joystick = joy;
	keyController = key;
}

void SelectorManager::setJoystick(Joystick* joy) {
	joystick = joy;
}


void SelectorManager::update(double deltaTime) {


	if (joystick) {
		if (joystick->aButtonPushed()) {
			controls[selected]->onClick();
			controls[selected]->onHover();
			timeSincePressed = DELAY_TIME;

		} else if (joystick->isUpPressed() || joystick->isLeftPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected - 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else if (joystick->isDownPressed() || joystick->isRightPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected + 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
		} else
			timeSincePressed = DELAY_TIME;
	}


	if (keyController) {
		if (keyController->isKeyPressed(Keyboard::Down)
			|| keyController->isKeyPressed(Keyboard::Right))
			setSelected(selected + 1);
		else if (keyController->isKeyPressed(Keyboard::Up)
			|| keyController->isKeyPressed(Keyboard::Left))
			setSelected(selected - 1);
		else if (keyController->isKeyPressed(Keyboard::Enter)) {
			controls[selected]->onClick();
			controls[selected]->onHover();
		}
	}
	for (int i = 0; i < controls.size(); ++i) {
		controls[i]->updateSelect(deltaTime);
		if (selected != i && controls[i]->hovering()) {
			setSelected(i);
		}
	}

	if (selected > -1)
		selector->update(deltaTime);

}

void SelectorManager::draw(SpriteBatch* batch) {
	for (const auto& control : controls)
		control->draw(batch);

	if (selected > -1)
		selector->draw(batch);
}

bool SelectorManager::hasController() {
	return joystick != NULL;
}

void SelectorManager::addControl(Selectable* control) {
	controls.push_back(control);

	if (selected == -1) {
		setSelected(0);
	}
}

void SelectorManager::addControls(vector<Selectable*> controls) {
	for (const auto& control : controls)
		controls.push_back(control);

	if (selected == -1) {
		setSelected(0);
	}
}


void SelectorManager::moveBy(const Vector2& moveVector) {
	for (const auto& control : controls)
		control->moveBy(moveVector);

	selector->moveBy(moveVector);
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
		selected = short(controls.size() - 1);
	else if (index > controls.size() - 1)
		selected = 0;
	else
		selected = index;

	selector->setDimensions(
		controls[selected]->getPosition(),
		Vector2((float) controls[selected]->getWidth(), (float) controls[selected]->getHeight()));

	controls[selected]->onHover();
}
