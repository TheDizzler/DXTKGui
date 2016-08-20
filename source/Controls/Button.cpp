#include "Button.h"


Button::Button() {
}

Button::~Button() {
}


void Button::load(unique_ptr<FontSet> font,
	ComPtr<ID3D11ShaderResourceView> pixelTexture) {

	frame.reset(new RectangleFrame(pixelTexture));
	rectSprite.reset(new RectangleSprite(pixelTexture));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), move(font)));

}

void Button::setDimensions(const Vector2& pos, const Vector2& size,
	const int frameThickness) {

	position = pos;
	hitArea->size = size;

	frame->setDimensions(pos, size, frameThickness);
	rectSprite->setDimensions(pos, size);
	setPosition(pos);

}


void Button::update(double deltaTime, MouseController* mouse) {

	if (hitArea->contains(mouse->getPosition())) {
		isHover = true;
		if (!isSelected)
			setToHoverState();
	} else
		isHover = false;

	if (isSelected && !mouse->leftButtonDown()) {
		isClicked = true;
		setToUnpressedState();
	} else {
		isClicked = false;
		if (!isHover) {
			isSelected = false;
			setToUnpressedState();
		} else if (!mouse->leftButtonLastDown() && mouse->leftButtonDown()) {
			isSelected = true;
			setToSelectedState();
		}
	}
}

void Button::setToUnpressedState() {

	rectSprite->setTint(normalColor);
	buttonLabel->setTint(normalColorText);
}

void Button::setToHoverState() {

	rectSprite->setTint(hoverColor);
	buttonLabel->setTint(hoverColorText);
}

void Button::setToSelectedState() {

	rectSprite->setTint(selectedColor);
	buttonLabel->setTint(selectedColorText);
}


void Button::draw(SpriteBatch* batch) {


	/*if (isSelected) {
		rectSprite->setTint(selectedColor);
		buttonLabel->setTint(selectedColorText);
	} else if (isHover) {
		rectSprite->setTint(hoverColor);
		buttonLabel->setTint(hoverColorText);
	}else {
		rectSprite->setTint(normalColor);
		buttonLabel->setTint(normalColorText);
	} */

	rectSprite->draw(batch);
	frame->draw(batch);
	buttonLabel->draw(batch);
}


void Button::setText(string text) {

	buttonLabel->setText(text);
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

	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());

	hitArea.reset(new HitArea(Vector2::Zero, size));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), move(font)));
}


void ImageButton::draw(SpriteBatch* batch) {

	//Sprite* drawSprite;
	//drawSprite = normalSprite.get();

	//if (isSelected) {
		//drawSprite = pressedSprite.get();
	//} else if (isHover)
		//buttonLabel->setTint(hoverColorText);
	//else
		//buttonLabel->setTint(normalColorText);

	drawSprite->draw(batch);
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


void ImageButton::setToUnpressedState() {
	buttonLabel->setTint(normalColorText);
	drawSprite = normalSprite.get();
}

void ImageButton::setToHoverState() {
	buttonLabel->setTint(hoverColorText);
	drawSprite = normalSprite.get();
}

void ImageButton::setToSelectedState() {

	drawSprite = pressedSprite.get();
}
