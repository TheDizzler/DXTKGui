#include "TextLabel.h"


TextLabel::TextLabel(Vector2 pos, shared_ptr<FontSet> fnt, wstring text) {

	position = pos;
	font = fnt;
	label = text;
}

TextLabel::TextLabel(Vector2 pos, shared_ptr<FontSet> fnt) {

	position = pos;
	font = fnt;
}


TextLabel::TextLabel(shared_ptr<FontSet> fnt) {

	font = fnt;
}

TextLabel::~TextLabel() {
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

const wchar_t* TextLabel::getText() {
	return label.c_str();
}


bool TextLabel::clicked() {

	if (isClicked) {
		isClicked = isSelected = false;
		return true;
	}

	return false;
}

bool TextLabel::selected() {

	return isSelected;
}

bool TextLabel::hovering() {
	return isHover;
}

#include "../Managers/GameManager.h"
void TextLabel::setFont(const pugi::char_t* fontName) {

	font = GameManager::guiFactory->getFont(fontName);
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


void TextLabel::update(double deltaTime, MouseController* mouse) {

	if (isHoverable) {
		isHover = hitArea->contains(mouse->getPosition());

		if (isSelected && !mouse->leftButtonDown()) {
			isClicked = true;
		} else {
			isClicked = false;
			if (!isHover)
				isSelected = false;
			else if (!mouse->leftButtonLastDown() && mouse->leftButtonDown()) {
				isSelected = true;
			}
		}
	}
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
