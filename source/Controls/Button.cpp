#include "Button.h"

using namespace Controls;

Button::Button() {
}

Button::~Button() {
}

bool Controls::Button::load(ID3D11Device* device, shared_ptr<FontSet> fnt,
	shared_ptr<ID3D11ShaderResourceView> pixelTexture) {


	return true;
}


void Button::update(double deltaTime, MouseController* mouse) {

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


void Button::setPosition(Vector2& pos) {

	position = pos;

	normalSprite->setPosition(position);
	pressedSprite->setPosition(position);

	//hitArea = normalSprite->getHitArea();
}

const Vector2& Button::getPosition() {
	return position;
}

void Button::setScale(const Vector2 & scale) {

	normalSprite->setScale(scale);
	pressedSprite->setScale(scale);

	hitArea = normalSprite->getHitArea();
}

int Button::getWidth() {

	return hitArea->size.x;
}

int Button::getHeight() {

	return hitArea->size.y;
}


bool Button::clicked() {

	if (isClicked) {
		isClicked = isSelected = false;
		return true;
	}

	return false;
}

bool Button::selected() {

	return isSelected;
}

bool Button::hovering() {
	return isHover;
}




/** **** TextButton **** **/
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
	buttonLabel->setPosition(Vector2(position.x - size.x / 2, position.y - size.y / 2 - 5));
}

void TextButton::setText(string text) {

	buttonLabel->setText(text);
}



/** **** ImageButton **** **/
ImageButton::ImageButton() {
}

ImageButton::~ImageButton() {
}

bool ImageButton::load(ID3D11Device * device, const wchar_t * upButtonFile,
	const wchar_t * downButtonFile) {


	normalSprite.reset(new Sprite());
	pressedSprite.reset(new Sprite());
	if (!normalSprite->load(device, upButtonFile) ||
		!pressedSprite->load(device, downButtonFile))
		return false;

	hitArea = normalSprite->getHitArea();

	return true;
}


void ImageButton::draw(SpriteBatch * batch) {

	Sprite* drawSprite;
	drawSprite = normalSprite.get();

	if (isSelected) {
		drawSprite = pressedSprite.get();
	} else if (isHover)
		drawSprite->setTint(hoverColor);
	else
		drawSprite->setTint(normalColor);

	drawSprite->draw(batch);
}