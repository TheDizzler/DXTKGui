#include "TextLabel.h"



TextLabel::TextLabel(Vector2 pos, FontSet* fnt) {

	/*wostringstream ws;
	ws << "Blank Texxt";
	wstring setupText = ws.str();
	label = setupText;*/
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
	label = ws.str();
}

void TextLabel::setText(wostringstream& text) {

	label = text.str();
}

void TextLabel::setText(wstring text) {

	label = text;
}

const wchar_t * TextLabel::getText() {
	return label.c_str();
}
