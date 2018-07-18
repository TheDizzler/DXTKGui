#include "Selector.h"
#include "..\GUIFactory.h"
#include "KeyboardController.h"

Selector::~Selector() {

}


GrowSelector::GrowSelector(GUIFactory* guiFactory) {
	frame.reset(guiFactory->createRectangleFrame());
	frame->setTint(Colors::White, false);

}

GrowSelector::~GrowSelector() {
}

void GrowSelector::reloadGraphicsAsset() {
	frame->reloadGraphicsAsset();

}

void GrowSelector::update(double deltaTime) {
	currentGrowTime += deltaTime;



	Vector2 newSize = Vector2::Lerp(originalSize, maxSize, float(currentGrowTime / maxGrowTime));
	frame->setSize(newSize);
	//Vector2 origin(newSize.x / 2, newSize.y / 2);
	//frame->setOrigin(origin);



	if (currentGrowTime >= maxGrowTime) {
		currentGrowTime = 0;
		newSize = originalSize;
		frame->setPosition(originalPos);
		frame->setSize(originalSize);
	} else {
		Vector2 posAdj((currentSize - newSize) / 2);
		frame->moveBy(posAdj);
	}

	frame->update();
	currentSize = newSize;
}

void GrowSelector::draw(SpriteBatch* batch) {
	frame->draw(batch);
}

void GrowSelector::setDimensions(const Vector2& pos, const Vector2& sz) {
	originalPos = pos;
	currentSize = originalSize = sz;
	maxSize = sz * growFactor;
	frame->setDimensions(pos, originalSize, frameThickness);
	currentGrowTime = 0;
}

void GrowSelector::moveBy(const Vector2& moveVector) {
	frame->moveBy(moveVector);
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
	selector = move(newSelector);
}

void SelectorManager::setControllers(Joystick* joy, KeyboardController* keys, MouseController* ms) {

	mouse = ms;
	keyController = keys;
	if (joy == NULL || joy->getControllerSockerNumber() == ControllerSocketNumber::DUMMY_SOCKET)
		joystick = NULL;
	else
		joystick = joy;

}

bool SelectorManager::setJoystick(Joystick* joy) {
	if (joy == NULL || joy->getControllerSockerNumber() == ControllerSocketNumber::DUMMY_SOCKET) {
		joystick = NULL;
		return false;
	} else {
		joystick = joy;
		return true;
	}
}

void SelectorManager::setMouse(MouseController* ms) {
	mouse = ms;
}

void SelectorManager::setKeys(KeyboardController* keys) {
	keyController = keys;
}


bool ignoreMouse = false;
bool SelectorManager::update(double deltaTime) {

	bool refreshNeeded = false;
	if (ignoreMouse)
		ignoreMouse = !mouse->hasMoved /*|| hasBeenClicked()*/;

	if (joystick) {
		if (joystick->aButtonPushed()) {
			controls[selected]->onClick();
			controls[selected]->onHover();
			timeSincePressed = DELAY_TIME;
			ignoreMouse = true;
		} else if (joystick->isUpPressed() || joystick->isLeftPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected - 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
			ignoreMouse = true;
		} else if (joystick->isDownPressed() || joystick->isRightPressed()) {

			if (timeSincePressed > DELAY_TIME) {
				setSelected(selected + 1);
				timeSincePressed = 0;
			}
			timeSincePressed += deltaTime;
			ignoreMouse = true;
		} else
			timeSincePressed = DELAY_TIME;
	}


	if (keyController) {
		if (!controls[selected]->isSelectLocked()) {
			if (keyController->isKeyPressed(Keyboard::Down)
				|| keyController->isKeyPressed(Keyboard::Right)) {
				setSelected(selected + 1);
				ignoreMouse = true;
			} else if (keyController->isKeyPressed(Keyboard::Up)
				|| keyController->isKeyPressed(Keyboard::Left)) {
				setSelected(selected - 1);
				ignoreMouse = true;
			} else if (keyController->isKeyPressed(Keyboard::Enter)) {
				//controls[selected]->onClick();
				controls[selected]->setSelectLock(true);
				controls[selected]->onHover();
				ignoreMouse = true;
			}
		} else {

			SelectableContainer* box = static_cast<SelectableContainer*>(controls[selected]);

			if (keyController->isKeyPressed(Keyboard::Down)
				|| keyController->isKeyPressed(Keyboard::Right)) {

				box->setHovered(box->getHoveredIndex() + 1);

			} else if (keyController->isKeyPressed(Keyboard::Up)
				|| keyController->isKeyPressed(Keyboard::Left)) {

				box->setHovered(box->getHoveredIndex() - 1);

			} else if (keyController->isKeyPressed(Keyboard::Enter)) {

				box->setSelected(box->getHoveredIndex());
				box->onClick();

			} else if (keyController->isKeyPressed(Keyboard::Escape)
			|| keyController->isKeyPressed(Keyboard::Tab)) {

				// un-Select Lock
				box->setSelectLock(false);
			}
		}
	}

	bool mouseHovering = false;
	for (int i = 0; i < controls.size(); ++i) {

		if (!ignoreMouse) {
			controls[i]->updateProjectedHitArea();
			if (controls[i]->getProjectedHitArea().contains(mouse->getPosition())) {
				mouseHovering = true;
				if (!selectedSetByMouse) {
					setSelected(i);
					selectedSetByMouse = true;
				}
				if (mouse->pressed()) {
					controls[selected]->onPress();
				} else if (mouse->clicked()) {
					controls[selected]->onClick();
					controls[selected]->onHover();
				}
			}
		}

		if (controls[i]->updateSelect(deltaTime))
			refreshNeeded = true;

		if (selected != i && controls[i]->hovering()) {
			setSelected(i);
		}
	}

	if (!mouseHovering)
		selectedSetByMouse = false;

	if (selected > -1)
		selector->update(deltaTime);

	return refreshNeeded;
}

void SelectorManager::draw(SpriteBatch* batch) {
	for (const auto& control : controls)
		control->draw(batch);

	if (selected > -1)
		selector->draw(batch);
}

void SelectorManager::drawWithoutSelector(SpriteBatch* batch) {
	for (const auto& control : controls)
		control->draw(batch);
}

void SelectorManager::drawSelector(SpriteBatch* batch) {
	if (selected > -1)
		selector->draw(batch);
}

size_t SelectorManager::size() {
	return controls.size();
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

void SelectorManager::addControls(vector<Selectable*> cntrls) {
	for (const auto& control : cntrls)
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

	selectedSetByMouse = false;
}

