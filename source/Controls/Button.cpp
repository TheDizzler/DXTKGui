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
	buttonLabel->setPosition(unpressedTextPosition);

}

void Button::setToHoverState() {

	rectSprite->setTint(hoverColor);
	buttonLabel->setTint(hoverColorText);
}

void Button::setToSelectedState() {

	rectSprite->setTint(selectedColor);
	buttonLabel->setTint(selectedColorText);
	buttonLabel->setPosition(pressedTextPosition);
}


void Button::draw(SpriteBatch* batch) {

	rectSprite->draw(batch);
	frame->draw(batch);
	buttonLabel->draw(batch);
}


void Button::setText(string text) {

	buttonLabel->setText(text);
}


void Button::setTextOffset(const Vector2& unpressedOffset,
	const Vector2& pressedOffset) {

	unpressedTextOffset = unpressedOffset;
	pressedTextOffset = pressedOffset;
}

void Button::setPosition(const Vector2& pos) {

	GUIControl::setPosition(pos);

	// center text
	Vector2 textsize = buttonLabel->measureString();
	//if (textsize.x > getWidth()) {
	//	int newTextWidth = .9; // text disappears when scale below 1!
	//	buttonLabel->setScale(Vector2(newTextWidth, newTextWidth));
	//	textsize = buttonLabel->measureString();
	//}
	//buttonLabel->setPosition(
		//Vector2(position.x - textsize.x / 2, position.y - textsize.y / 2 - 5));
	Vector2 newPos = Vector2(position.x - textsize.x / 2,
		position.y - textsize.y / 2);
	unpressedTextPosition = newPos;
	unpressedTextPosition += unpressedTextOffset;
	pressedTextPosition = newPos;
	pressedTextPosition += pressedTextOffset;
	buttonLabel->setPosition(unpressedTextPosition);

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

	buttonLabel->setFont(move(newFont));
}




/** **** ImageButton **** **/
ImageButton::ImageButton() {

	// a rough guesstimate
	setTextOffset(Vector2(0, -5), Vector2(0, 0));
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
	Vector2 pos = buttonLabel->getPosition();
	buttonLabel->setPosition(unpressedTextPosition);
	drawSprite = normalSprite.get();
}

void ImageButton::setToHoverState() {
	buttonLabel->setTint(hoverColorText);
	drawSprite = normalSprite.get();
}

void ImageButton::setToSelectedState() {

	Vector2 pos = buttonLabel->getPosition();
	buttonLabel->setPosition(pressedTextPosition);
	drawSprite = pressedSprite.get();
}
