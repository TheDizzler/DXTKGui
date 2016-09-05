#include "CheckBox.h"

CheckBox::CheckBox(unique_ptr<Sprite> unchkdSprite,
	unique_ptr<Sprite> chckdSprite, unique_ptr<FontSet> font) {

	uncheckedSprite = move(unchkdSprite);
	checkedSprite = move(chckdSprite);

	uncheckedSprite->setOrigin(Vector2(0, 0));
	checkedSprite->setOrigin(Vector2(0, 0));
	Vector2 size = Vector2(uncheckedSprite->getWidth(), uncheckedSprite->getHeight());

	hitArea.reset(new HitArea(Vector2::Zero, size));

	label.reset(new TextLabel(move(font)));
	label->setText(L"");
}

CheckBox::~CheckBox() {

	if (onClickListener != NULL)
		delete onClickListener;
}

void CheckBox::draw(SpriteBatch* batch) {

	drawSprite->draw(batch);
	label->draw(batch);
}

void CheckBox::update(double deltaTime, MouseController* mouse) {

	if (hitArea->contains(mouse->getPosition())
		|| label->contains(mouse->getPosition())) {
		isHover = true;
		label->setTint(hoverColorText);
	} else {
		label->setTint(normalColorText);
		isHover = false;
	}

	if (isHover) {
		if (mouse->pressed()) {
			isClicked = !isClicked;
			triggerOnClick();
		}
	}

	if (isClicked)
		drawSprite = checkedSprite.get();
	else
		drawSprite = uncheckedSprite.get();

}

#include "../Controls/GUIFactory.h"
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


void CheckBox::setChecked(bool checked)  {
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


XMVECTOR XM_CALLCONV CheckBox::measureString() const {
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

bool CheckBox::selected() {
	return isSelected;
}

bool CheckBox::hovering() {
	return isHover;
}
