#include "Button.h"


Button::Button(GraphicsAsset* pixelAsset,
	unique_ptr<FontSet> font) {

	if (pixelAsset != NULL) {
		// this it stop errors when coming from ImageButton
		frame.reset(new RectangleFrame(pixelAsset));
		rectSprite.reset(new RectangleSprite(pixelAsset));
	}
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	buttonLabel.reset(new TextLabel(Vector2(0, 0), L"", move(font)));

	position = Vector2(-1, -1);

	if (pixelAsset != NULL)
		setToUnpressedState();	// this always call Button::setToUnpressedState
								// even if it is an ImageButton

}


Button::~Button() {
	if (onClickListener != NULL)
		delete onClickListener;
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

#include "../Controls/GUIFactory.h"
void Button::setFont(const pugi::char_t* font) {

	buttonLabel->setFont(guiFactory->getFont(font));
}




ImageButton::ImageButton(unique_ptr<Sprite> buttonSprite,
	unique_ptr<FontSet> font) : Button(NULL, move(font)) {

	// a rough guesstimate
	setTextOffset(Vector2(0, -5), Vector2(0, 0));

	normalSprite = move(buttonSprite);
	//normalSprite->setOrigin(Vector2(0, 0));
	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());

	hitArea.reset(new HitArea(Vector2::Zero, size));

	setToUnpressedState();
}

/** **** ImageButton **** **/
ImageButton::ImageButton(unique_ptr<Sprite> upButtonSprite,
	unique_ptr<Sprite> downButtonSprite, unique_ptr<FontSet> font)
	: Button(NULL, move(font)) {

	// a rough guesstimate
	setTextOffset(Vector2(0, -5), Vector2(0, 0));

	normalSprite = move(upButtonSprite);
	pressedSprite = move(downButtonSprite);
	//normalSprite->setOrigin(Vector2(0, 0));
	//pressedSprite->setOrigin(Vector2(0, 0));
	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());

	hitArea.reset(new HitArea(Vector2::Zero, size));
	setToUnpressedState();
}


ImageButton::~ImageButton() {
}


void ImageButton::draw(SpriteBatch* batch) {

	//drawSprite->draw(batch);
	batch->Draw(texture, normalSprite->getPosition(), &normalSprite->getRect(),
		tint, rotation, normalSprite->getOrigin(), scale, SpriteEffects_None, layerDepth);
	buttonLabel->draw(batch);
}

void ImageButton::setPosition(const Vector2& pos) {

	position = pos;
	Button::setPosition(position);
	Vector2 spritePos = position;
	spritePos.x += normalSprite->getWidth()/2;
	spritePos.y += normalSprite->getHeight() / 2;
	normalSprite->setPosition(spritePos);
	//if (pressedSprite != NULL)
		//pressedSprite->setPosition(position);

}

void ImageButton::setScale(const Vector2& scl) {

	Button::setScale(scl);
	normalSprite->setScale(scale);
	if (pressedSprite != NULL)
		pressedSprite->setScale(scale);
}

void ImageButton::setRotation(const float rot) {
	rotation = rot;
}


void ImageButton::setToUnpressedState() {

	buttonLabel->setTint(normalColorText);
	buttonLabel->setPosition(unpressedTextPosition);
	tint = normalColor;
	texture = normalSprite->getTexture().Get();
}

void ImageButton::setToHoverState() {

	buttonLabel->setTint(hoverColorText);
	tint = hoverColor;
	texture = normalSprite->getTexture().Get();
}

void ImageButton::setToSelectedState() {

	buttonLabel->setPosition(pressedTextPosition);
	if (pressedSprite.get() != NULL)
		texture = pressedSprite->getTexture().Get();
	else
		tint = selectedColor;
}
