#include "TextLabel.h"


TextLabel::TextLabel(Vector2 pos, wstring text, shared_ptr<FontSet> fnt) {

	position = pos;
	font = fnt;
	setText(text);
}

TextLabel::TextLabel(Vector2 pos, shared_ptr<FontSet> fnt) {

	position = pos;
	font = fnt;
}


TextLabel::TextLabel(shared_ptr<FontSet> fnt) {

	font = fnt;
}

TextLabel::~TextLabel() {
	if (onClickListener != NULL)
		delete onClickListener;
	if (onHoverListener != NULL)
		delete onHoverListener;
}


void TextLabel::update(double deltaTime, MouseController* mouse) {

	if (isHoverable) {
		if (hitArea->contains(mouse->getPosition())) {
			isHover = true;
			if (!isPressed) {
				onHover();
				setToHoverState();
			}
		} else
			isHover = false;

		if (isPressed && !mouse->leftButtonDown()) {
			isClicked = true;
			onClick();
			setToUnpressedState();
		} else {
			isClicked = false;
			if (!isHover) {
				isPressed = false;
				setToUnpressedState();
			} else if (mouse->clicked()) {
				isPressed = true;
				setToSelectedState();
			}
		}
	}
}


void TextLabel::draw(SpriteBatch* batch) {

	font->draw(batch, label.c_str(), position);
}

void TextLabel::draw(SpriteBatch* batch, Color color) {

	font->draw(batch, label.c_str(), position, color);
}

void TextLabel::setText(string text) {

	wostringstream ws;
	ws << text.c_str();
	setText(ws);
}

void TextLabel::setText(wostringstream& text) {

	setText(text.str());
}

void TextLabel::setText(wstring text) {

	label = text;
	Vector2 size = font->measureString(label.c_str());
	hitArea.reset(new HitArea(position, size));
}

XMVECTOR XM_CALLCONV TextLabel::measureString() const {
	return font->measureString(label.c_str());
}

Vector2 XM_CALLCONV TextLabel::measureString(wstring string) const {
	return font->measureString(string.c_str());
}

const wchar_t* TextLabel::getText() {
	return label.c_str();
}


bool TextLabel::clicked() {

	if (isClicked) {
		isClicked = isPressed = false;
		return true;
	}

	return false;
}

bool TextLabel::pressed() {

	return isPressed;
}

bool TextLabel::hovering() {
	return isHover;
}

void TextLabel::setHoverable(bool hoverable) {
	isHoverable = hoverable;
}

void TextLabel::setToUnpressedState() {
	font->setTint(normalColorText);
}

void TextLabel::setToHoverState() {
	font->setTint(hoverColorText);
}

void TextLabel::setToSelectedState() {
	font->setTint(selectedColorText);
}

#include "../Controls/GUIFactory.h"
void TextLabel::setFont(const pugi::char_t* fontName) {

	font = guiFactory->getFont(fontName);
}

void TextLabel::setFont(shared_ptr<FontSet> newFont) {

	font = newFont;
}

void TextLabel::setTint(const Color& color) {

	tint = color; // this kind of irrelevant, but oh well
	font->setTint(color);
}

/** Warning: this will affect all text sharing this font! */
void TextLabel::setScale(const Vector2 & scl) {
	font->setScale(scl);
}

const Vector2& TextLabel::getPosition() const {
	return position;
}


int const TextLabel::getWidth() const {

	return hitArea->size.x;
}

int const TextLabel::getHeight() const {
	return hitArea->size.y;
}
