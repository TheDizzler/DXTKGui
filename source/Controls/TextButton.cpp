#include "TextButton.h"

TextButton::TextButton() {

}

TextButton::~TextButton() {
}


bool TextButton::load(ID3D11Device* device, const wchar_t* fontFile,
	const wchar_t* upButtonFile, const wchar_t* downButtonFile) {

	buttonFont.reset(new FontSet());
	if (!buttonFont->load(device, fontFile))
		return false;
	buttonFont->setTint(normalColor);

	buttonLabel.reset(new TextLabel(
		Vector2(0, 0), buttonFont.get()));

	normalSprite.reset(new Sprite());
	pressedSprite.reset(new Sprite());
	if (!normalSprite->load(device, upButtonFile) ||
		!pressedSprite->load(device, downButtonFile))
		return false;

	hitArea = normalSprite->getHitArea();

	return true;
}





void TextButton::draw(SpriteBatch* batch) {

	Sprite* drawSprite;
	drawSprite = normalSprite.get();

	if (isSelected) {
		drawSprite = pressedSprite.get();
		buttonFont->setTint(selectedColor);
	} else if (isHover)
		buttonFont->setTint(hoverColor);
	else
		buttonFont->setTint(normalColor);

	drawSprite->draw(batch);
	buttonLabel->draw(batch);

}


void TextButton::setPosition(Vector2& position) {

	Button::setPosition(position);
	Vector2 size = buttonFont->measureString(buttonLabel->getText());
	buttonLabel->position = Vector2(position.x - size.x / 2, position.y - size.y / 2 - 5);
}

void TextButton::setText(string text) {

	buttonLabel->setText(text);
}

