#include "TextLabel.h"


TextLabel::TextLabel(Vector2 pos, wstring text, shared_ptr<FontSet> fnt) {

	position = pos;
	font = fnt;
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	setText(text);
}

TextLabel::TextLabel(Vector2 pos, shared_ptr<FontSet> fnt) {

	position = pos;
	font = fnt;
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
}


TextLabel::TextLabel(shared_ptr<FontSet> fnt) {

	font = fnt;
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
}

TextLabel::~TextLabel() {
	if (onClickListener != NULL)
		delete onClickListener;
	if (onHoverListener != NULL)
		delete onHoverListener;
}


void TextLabel::update(double deltaTime) {

	if (isHoverable) {
		if (hitArea->contains(mouse->getPosition())) {
			isHover = true;
			if (!isPressed) {
				onHover();
				setToHoverState();
			}
		} else
			isHover = false;

		if (isPressed && !mouse->leftButton()) {
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

	font->draw(batch, label.c_str(), position, tint,
		rotation, origin, scale);
}

void TextLabel::draw(SpriteBatch* batch, Color color) {
	font->draw(batch, label.c_str(), position, color,
		rotation, origin, scale);
}

#include <sstream>
void TextLabel::setText(string text) {

	wostringstream wss;
	wss << text.c_str();
	setText(wss.str());
}

void TextLabel::setText(wostringstream& text) {
	setText(text.str());
}

void TextLabel::setText(wstring text) {

	label = text;
	Vector2 size = font->measureString(label.c_str());
	size *= scale;
	hitArea->position = position;
	hitArea->size = size;
}

const Vector2& XM_CALLCONV TextLabel::measureString() const {
	Vector2 size = font->measureString(label.c_str());
	Vector2 scaledSize = size*scale;
	return scaledSize;
}

const Vector2& XM_CALLCONV TextLabel::measureString(wstring string) const {
	Vector2 size = font->measureString(string.c_str());
	return size;
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

#include "GUIFactory.h"
void TextLabel::setFont(const pugi::char_t* fontName) {
	font = guiFactory->getFont(fontName);
}

void TextLabel::setFont(shared_ptr<FontSet> newFont) {
	font = newFont;
}

void TextLabel::setTint(const XMFLOAT4 color) {
	tint = color;
}

void TextLabel::setScale(const Vector2 & scl) {
	scale = scl;
	Vector2 size = font->measureString(label.c_str());
	size *= scale;
	hitArea->position = position;
	hitArea->size = size;
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

const shared_ptr<FontSet> TextLabel::getFont() const {
	return font;
}
