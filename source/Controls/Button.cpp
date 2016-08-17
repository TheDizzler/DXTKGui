#include "Button.h"


Button::Button() {
}

Button::~Button() {
}


void Button::load(unique_ptr<FontSet> font,
	ComPtr<ID3D11ShaderResourceView> pixelTexture) {

	//pixel = pixelTexture;
	rectSprite.reset(new RectangleSprite(pixelTexture));

	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), move(font)));
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


void Button::draw(SpriteBatch * batch) {


	if (isSelected) {
		rectSprite->setTint(selectedColor);
		buttonLabel->setTint(selectedColorText);
	} else if (isHover) {
		rectSprite->setTint(hoverColor);
		buttonLabel->setTint(hoverColorText);
	} else {
		rectSprite->setTint(normalColor);
		buttonLabel->setTint(normalColorText);
	}

	rectSprite->draw(batch);
	buttonLabel->draw(batch);
}


void Button::setText(string text) {

	buttonLabel->setText(text);
}


void Button::setDimensions(const Vector2& pos, const Vector2& size) {

	position = pos;

	/*buttonRect.left = 0;
	buttonRect.top = 0;
	buttonRect.right = size.x * scale.x;
	buttonRect.bottom = size.y * scale.y;*/
	rectSprite->setDimensions(pos, size);

	setPosition(pos);
	hitArea->size = size;
}

void Button::setPosition(const Vector2& pos) {

	GUIControl::setPosition(pos);

	// center text
	Vector2 textsize = buttonLabel->measureString();
	buttonLabel->setPosition(
		Vector2(position.x - textsize.x / 2, position.y - textsize.y / 2 - 5));

}

const Vector2& Button::getPosition() const {
	return position;
}

void Button::setScale(const Vector2& scl) {

	GUIControl::setScale(scl);
	buttonLabel->setScale(scale);

}

const int Button::getWidth() const {

	return hitArea->size.x;
}

const int Button::getHeight() const {

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

void Button::setFont(unique_ptr<FontSet> newFont) {

	//buttonFont.release();
	buttonLabel->setFont(move(newFont));
}




/** **** ImageButton **** **/
ImageButton::ImageButton() {
}

ImageButton::~ImageButton() {
}


void ImageButton::load(unique_ptr<FontSet> font, unique_ptr<Sprite> upButtonSprite,
	unique_ptr<Sprite> downButtonSprite) {

	normalSprite = move(upButtonSprite);
	pressedSprite = move(downButtonSprite);

	Vector2 size = Vector2(normalSprite->getWidth(), pressedSprite->getHeight());

	hitArea.reset(new HitArea(Vector2::Zero, size));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), move(font)));
}


void ImageButton::draw(SpriteBatch* batch) {

	Sprite* drawSprite;
	drawSprite = normalSprite.get();

	if (isSelected) {
		drawSprite = pressedSprite.get();
	} else if (isHover)
		buttonLabel->setTint(hoverColorText);
	else
		buttonLabel->setTint(normalColorText);

	//drawSprite->draw(batch);
	batch->Draw(drawSprite->getTexture().Get(), position, &drawSprite->getRect(),
		normalColor, rotation, origin, scale, SpriteEffects_None, layerDepth);
	buttonLabel->draw(batch);
}

void ImageButton::setPosition(const Vector2& pos) {

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