#include "Button.h"


Button::Button() {
}

Button::~Button() {
}


void Button::load(shared_ptr<FontSet> font,
	ComPtr<ID3D11ShaderResourceView> pixelTexture) {

	buttonFont = font;
	pixel = pixelTexture;

	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), buttonFont));
}

void Button::update(double deltaTime, MouseController* mouse) {

	isHover = hitArea->contains(mouse->getPosition());

	if (isHover)
		int x = 1;

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


void Button::draw(SpriteBatch * batch) {


	if (isSelected) {
		batch->Draw(pixel.Get(), position, &buttonRect,
			selectedColor, 0.0f, Vector2(0, 0), scale,
			SpriteEffects_None, 0.0f);
		buttonFont->setTint(selectedColorText);
	} else if (isHover) {
		batch->Draw(pixel.Get(), position, &buttonRect,
			hoverColor, 0.0f, Vector2(0, 0), scale,
			SpriteEffects_None, 0.0f);
		buttonFont->setTint(hoverColorText);
	} else {
		batch->Draw(pixel.Get(), position, &buttonRect,
			normalColor, 0.0f, Vector2(0, 0), scale,
			SpriteEffects_None, 0.0f);
		buttonFont->setTint(normalColorText);
	}


	buttonLabel->draw(batch);
}


void Button::setText(string text) {

	buttonLabel->setText(text);
}


void Button::setDimensions(const Vector2& pos, const Vector2& size) {

	position = pos;

	buttonRect.left = 0;
	buttonRect.top = 0;
	buttonRect.right = size.x * scale.x;
	buttonRect.bottom = size.y * scale.y;

	hitArea.reset(new HitArea(
		position, Vector2(buttonRect.right, buttonRect.bottom)));

}

void Button::setPosition(Vector2& pos) {

	GUIControl::setPosition(pos);

	Vector2 textsize = buttonFont->measureString(buttonLabel->getText());
	buttonLabel->setPosition(
		Vector2(position.x - textsize.x / 2, position.y - textsize.y / 2 - 5));

}

const Vector2& Button::getPosition() {
	return position;
}

void Button::setScale(const Vector2& scl) {

	GUIControl::setScale(scl);
	buttonLabel->setScale(scale);

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

void Button::setFont(shared_ptr<FontSet> newFont) {

	buttonFont = newFont;
}




/** **** ImageButton **** **/
ImageButton::ImageButton() {
}

ImageButton::~ImageButton() {
}


void ImageButton::load(shared_ptr<FontSet> fnt,
	shared_ptr<Sprite> upButtonSprite, shared_ptr<Sprite> downButtonSprite) {

	buttonFont = fnt;
	normalSprite = upButtonSprite;
	pressedSprite = downButtonSprite;

	hitArea.reset(new HitArea(normalSprite->getHitArea()->position,
		normalSprite->getHitArea()->size));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), buttonFont));
}


void ImageButton::draw(SpriteBatch* batch) {

	Sprite* drawSprite;
	drawSprite = normalSprite.get();

	if (isSelected) {
		drawSprite = pressedSprite.get();
	} else if (isHover)
		buttonFont->setTint(hoverColorText);
	else
		buttonFont->setTint(normalColorText);

	drawSprite->draw(batch);
	buttonLabel->draw(batch);
}

void ImageButton::setPosition(Vector2& pos) {

	Button::setPosition(pos);
	normalSprite->setPosition(position);
	pressedSprite->setPosition(position);

}

void ImageButton::setScale(const Vector2& scl) {

	Button::setScale(scl);
	normalSprite->setScale(scale);
	pressedSprite->setScale(scale);
}



/** **** TextButton **** **/
//TextButton::TextButton() {
//
//}
//
//TextButton::~TextButton() {
//}


//bool TextButton::load(ID3D11Device* device, const wchar_t* fontFile,
//	const wchar_t* upButtonFile, const wchar_t* downButtonFile) {
//
//	buttonFont.reset(new FontSet());
//	if (!buttonFont->load(device, fontFile))
//		return false;
//	buttonFont->setTint(normalColor);
//
//	buttonLabel.reset(new TextLabel(
//		Vector2(0, 0), buttonFont.get()));
//
//	normalSprite.reset(new Sprite());
//	pressedSprite.reset(new Sprite());
//	if (!normalSprite->load(device, upButtonFile) ||
//		!pressedSprite->load(device, downButtonFile))
//		return false;
//
//	hitArea = normalSprite->getHitArea();
//
//	return true;
//}


//void TextButton::draw(SpriteBatch* batch) {
//
//	Sprite* drawSprite;
//	drawSprite = normalSprite.get();
//
//	if (isSelected) {
//		drawSprite = pressedSprite.get();
//		buttonFont->setTint(selectedColor);
//	} else if (isHover)
//		buttonFont->setTint(hoverColor);
//	else
//		buttonFont->setTint(normalColor);
//
//	drawSprite->draw(batch);
//	buttonLabel->draw(batch);
//
//}
//
//
//void TextButton::setPosition(Vector2& position) {
//
//	Button::setPosition(position);
//	Vector2 size = buttonFont->measureString(buttonLabel->getText());
//	buttonLabel->setPosition(Vector2(position.x - size.x / 2, position.y - size.y / 2 - 5));
//}
//
//void TextButton::setText(string text) {
//
//	buttonLabel->setText(text);
//}