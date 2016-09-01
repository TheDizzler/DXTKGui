#include "Button.h"


Button::Button(ComPtr<ID3D11ShaderResourceView> pixelTexture,
	unique_ptr<FontSet> font) {

	frame.reset(new RectangleFrame(pixelTexture));
	rectSprite.reset(new RectangleSprite(pixelTexture));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), L"", move(font)));

	position = Vector2(-1, -1);
}


Button::~Button() {
}


int textMargin = 5;
bool resized = false;
void Button::setDimensions(const Vector2& pos, const Vector2& size,
	const int frmThcknss) {

	//position = pos;
	frameThickness = frmThcknss;

	Vector2 labelSize = measureString();
	Vector2 newSize = size;
	if ((labelSize.x + textMargin * 2) > size.x
		|| (labelSize.y + textMargin * 2) > size.y) {

		if ((labelSize.x + textMargin * 2) > size.x)
			newSize.x = labelSize.x + textMargin * 2;
		if ((labelSize.y + textMargin * 2) > size.y)
			newSize.y = labelSize.y + textMargin * 2;
		resized = true;
	}
	hitArea->size = newSize;

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
		triggerOnClick();
		setToUnpressedState();
	} else {
		isClicked = false;
		if (!isHover) {
			isSelected = false;
			setToUnpressedState();
		} else if (mouse->pressed()) {
			isSelected = true;
			setToSelectedState();
		}
	}
}

void Button::draw(SpriteBatch* batch) {

	rectSprite->draw(batch);
	frame->draw(batch);
	buttonLabel->draw(batch);
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


void Button::setText(wstring text) {

	buttonLabel->setText(text);

	// SET POSITION if dimensions have been set
	if (position != Vector2(-1, -1))
		setDimensions(position, hitArea->size, frame->getThickness());
}

XMVECTOR XM_CALLCONV Button::measureString() const {
	return buttonLabel->measureString();
}


void Button::setTextOffset(const Vector2& unpressedOffset,
	const Vector2& pressedOffset) {

	unpressedTextOffset = unpressedOffset;
	pressedTextOffset = pressedOffset;
}

void Button::setPosition(const Vector2& pos) {

	GUIControl::setPosition(pos);

	if (frame != NULL)
		frame->setDimensions(position, hitArea->size, frameThickness);
	if (rectSprite != NULL)
		rectSprite->setDimensions(position, hitArea->size);

	// center text
	positionText();

}

void Button::positionText() {

	Vector2 textsize = buttonLabel->measureString();
	if (textsize.x > 0) {

		Vector2 newPos;
		if (resized) {
			newPos = Vector2(
				position.x + (getWidth() - textsize.x) / 2 - textMargin,
				position.y + (getHeight() - textsize.y) / 2 - textMargin);
			resized = false;
		} else
			newPos = Vector2(
				position.x + (getWidth() - textsize.x) / 2,
				position.y + (getHeight() - textsize.y) / 2);


		unpressedTextPosition = newPos;
		unpressedTextPosition += unpressedTextOffset;
		pressedTextPosition = newPos;
		pressedTextPosition += pressedTextOffset;
		buttonLabel->setPosition(unpressedTextPosition);
	}
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

#include "../Managers/GameManager.h"
void Button::setFont(const pugi::char_t* font) {

	buttonLabel->setFont(GameManager::guiFactory->getFont(font));
}




/** **** ImageButton **** **/
ImageButton::ImageButton(unique_ptr<Sprite> upButtonSprite,
	unique_ptr<Sprite> downButtonSprite, unique_ptr<FontSet> font)
	: Button(NULL, move(font)) {

	// a rough guesstimate
	setTextOffset(Vector2(0, -5), Vector2(0, 0));

	normalSprite = move(upButtonSprite);
	pressedSprite = move(downButtonSprite);
	normalSprite->setOrigin(Vector2(0, 0));
	pressedSprite->setOrigin(Vector2(0, 0));
	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());

	hitArea.reset(new HitArea(Vector2::Zero, size));
}

ImageButton::~ImageButton() {
}


void ImageButton::draw(SpriteBatch* batch) {

	drawSprite->draw(batch);
	buttonLabel->draw(batch);
}

void ImageButton::setPosition(const Vector2& pos) {

	position = pos;
	Button::setPosition(position);
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
	normalSprite->setTint(normalColor);
	drawSprite = normalSprite.get();
}

void ImageButton::setToHoverState() {

	buttonLabel->setTint(hoverColorText);
	normalSprite->setTint(hoverColor);
	drawSprite = normalSprite.get();
}

void ImageButton::setToSelectedState() {

	Vector2 pos = buttonLabel->getPosition();
	buttonLabel->setPosition(pressedTextPosition);
	drawSprite = pressedSprite.get();
}
