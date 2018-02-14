#include "CheckBox.h"
#include "../GUIFactory.h"

CheckBox::CheckBox(GUIFactory* factory, MouseController* mouseController,
	unique_ptr<Sprite> unchkdSprite, unique_ptr<Sprite> chckdSprite,
	const pugi::char_t* font) : GUIControl(factory, mouseController) {

	uncheckedSprite = move(unchkdSprite);
	checkedSprite = move(chckdSprite);

	uncheckedSprite->setOrigin(Vector2(0, 0));
	checkedSprite->setOrigin(Vector2(0, 0));
	Vector2 size = Vector2((float) uncheckedSprite->getWidth(), (float) uncheckedSprite->getHeight());

	hitArea.size = size;

	label.reset(guiFactory->createTextLabel(Vector2::Zero, L"", font));

	texture = uncheckedSprite->getTexture();
	currentRECT = uncheckedSprite->getRect();
}

CheckBox::~CheckBox() {

	if (actionListener != NULL)
		delete actionListener;

}

void CheckBox::reloadGraphicsAsset() {
	texture.Reset();
	uncheckedSprite->reloadGraphicsAsset(guiFactory);
	checkedSprite->reloadGraphicsAsset(guiFactory);
	label->reloadGraphicsAsset();

	if (isClicked) {
		texture = checkedSprite->getTexture();
		currentRECT = checkedSprite->getRect();
	} else {
		texture = uncheckedSprite->getTexture();
		currentRECT = uncheckedSprite->getRect();
	}
}
#include "../StringHelper.h"
bool CheckBox::update(double deltaTime) {

	refreshed = false;
	if (hitArea.contains(mouse->getPosition()) || label->contains(mouse->getPosition())) {

		if (!isHover) {
			isHover = true;
			onHover();
		}
	} else if (isHover) {
		resetState();
		isHover = false;
	}

	if (isHover) {
		if (mouse->pressed()) {
			isClicked = !isClicked;
			onClick();
		}
	}
	if (label->update(deltaTime))
		refreshed = true;

	return refreshed;
}

void CheckBox::draw(SpriteBatch* batch) {

	batch->Draw(texture.Get(), position, &currentRECT, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);
	label->draw(batch);
}


void CheckBox::setFont(const pugi::char_t* font) {
	label->setFont(guiFactory->getFont(font));
}


void CheckBox::setText(wstring text) {
	label->setText(text);
	centerText();
}


void CheckBox::setPosition(const Vector2& pos) {

	GUIControl::setPosition(pos);

	uncheckedSprite->setPosition(position);
	checkedSprite->setPosition(position);

	centerText();
}


void CheckBox::setChecked(bool checked) {
	isClicked = checked;
}

void CheckBox::centerText() {

	Vector2 labelSize = label->measureString();
	labelOffset.y = (getHeight() - labelSize.y) / 2;

	Vector2 labelPos = position;
	labelPos.x += labelOffset.x + getWidth();
	labelPos.y += labelOffset.y;
	label->setPosition(labelPos);
}

void CheckBox::setScale(const Vector2& scl) {

	GUIControl::setScale(scl);
	uncheckedSprite->setScale(scale);
	checkedSprite->setScale(scale);
}


const Vector2 XM_CALLCONV CheckBox::measureString() const {
	return label->measureString();
}

const Vector2& CheckBox::getPosition() const {
	return position;
}

const int CheckBox::getWidth() const {
	return uncheckedSprite->getWidth();
}

const int CheckBox::getHeight() const {
	return uncheckedSprite->getHeight();
}

bool CheckBox::clicked() {
	return isClicked;
}

bool CheckBox::pressed() {
	return isPressed;
}

bool CheckBox::hovering() {
	return isHover;
}
