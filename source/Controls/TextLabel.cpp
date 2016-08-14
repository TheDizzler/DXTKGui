#include "TextLabel.h"


using namespace Controls;


TextLabel::TextLabel(Vector2 pos, FontSet* fnt, wstring text) {

	position = pos;
	font = fnt;
	label = text;
}

TextLabel::TextLabel(Vector2 pos, FontSet* fnt) {

	position = pos;
	font = fnt;
}


TextLabel::TextLabel(FontSet* fnt) {

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

	size = font->measureString(label.c_str());
	label = text;
}

const wchar_t * TextLabel::getText() {
	return label.c_str();
}

void Controls::TextLabel::update(double deltaTime, MouseController* mouse) {
}

void Controls::TextLabel::setPosition(Vector2& position) {
}

const Vector2& Controls::TextLabel::getPosition() {
	// TODO: insert return statement here
}

int Controls::TextLabel::getWidth() {
	return size.x;
}

int Controls::TextLabel::getHeight() {
	return size.y;
}
