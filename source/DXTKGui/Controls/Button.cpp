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
	if (onHoverListener != NULL)
		delete onHoverListener;
}


int textMargin = 10;
bool resized = false;
void Button::setDimensions(const Vector2& pos, const Vector2& size,
	const int frmThcknss) {

	frameThickness = frmThcknss;

	Vector2 labelSize = measureString();
	Vector2 newSize = size;

	if ((labelSize.x + textMargin * 2) > size.x) {
		newSize.x = labelSize.x + textMargin * 2;
		resized = true;
	}
	if ((labelSize.y + textMargin * 2) > size.y) {
		newSize.y = labelSize.y + textMargin * 2;
		resized = true;
	}

	hitArea->size = newSize;

	width = newSize.x;
	height = newSize.y;

	setPosition(pos);

}


void Button::update(double deltaTime) {

	if (hitArea->contains(mouse->getPosition())) {
		isHover = true;
		if (!isPressed) {
			onHover();
			setToHoverState();
		}
	} else
		isHover = false;

	if (isPressed && !mouse->leftButton()) {
		isClicked = true;
		onClick();
		setToUnpressedState();
	} else {
		isClicked = false;
		if (!isHover) {
			isPressed = false;
			setToUnpressedState();
		} else if (mouse->pressed()) {
			isPressed = true;
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

const wchar_t* Button::getText() {
	return buttonLabel->getText();
}

const Vector2& XM_CALLCONV Button::measureString() const {
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
				position.x + (getScaledWidth() - textsize.x) / 2 /*+ textMargin*/,
				position.y + (getScaledHeight() - textsize.y) / 2 /*+ textMargin*/);
			resized = false;
		} else
			newPos = Vector2(
				position.x + (getScaledWidth() - textsize.x) / 2,
				position.y + (getScaledHeight() - textsize.y) / 2);


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
	if (frame != NULL)
		frame->setScale(scl);
	if (rectSprite != NULL)
		rectSprite->setScale(scl);
	buttonLabel->setScale(scale);

}


const int Button::getWidth() const {

	return width;
}

const int Button::getHeight() const {

	return height;
}

const int Button::getScaledWidth() const {
	return hitArea->size.x;
}

const int Button::getScaledHeight() const {
	return hitArea->size.y;
}


bool Button::clicked() {

	if (isClicked) {
		isClicked = isPressed = false;
		return true;
	}

	return false;
}

bool Button::pressed() {

	return isPressed;
}

bool Button::hovering() {
	return isHover;
}

#include "GUIFactory.h"
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
	width = size.x;
	height = size.x;
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

	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());
	width = size.x;
	height = size.y;
	hitArea.reset(new HitArea(Vector2::Zero, size));
	setToUnpressedState();
}


ImageButton::~ImageButton() {
}


void ImageButton::draw(SpriteBatch* batch) {

	batch->Draw(texture, normalSprite->getPosition(), &normalSprite->getRect(),
		tint, rotation, normalSprite->getOrigin(), scale, SpriteEffects_None, layerDepth);
	buttonLabel->draw(batch);
}

void ImageButton::setPosition(const Vector2& pos) {

	position = pos;
	Button::setPosition(position);
	Vector2 spritePos = position;
	spritePos.x += normalSprite->getWidth() / 2;
	spritePos.y += normalSprite->getHeight() / 2;
	normalSprite->setPosition(spritePos);


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
/** ***** END OF IMAGE BUTTON **** **/



/** ***** Animated Button ***** **/
AnimatedButton::AnimatedButton(shared_ptr<Animation> anim, Vector2 pos) {

	animation = anim;

	if (animation->animationFrames.size() > 0)
		currentFrameIndex = 0;

	position = pos;
	hitArea.reset(new HitArea(position, Vector2(getWidth(), getHeight())));
	center = Vector2(getWidth() / 2, getHeight() / 2);
}

AnimatedButton::~AnimatedButton() {
	if (onClickListener != NULL)
		delete onClickListener;

}


void AnimatedButton::update(double deltaTime) {

	if (hitArea->contains(mouse->getPosition())) {
		isHover = true;
		if (!isPressed) {
			onHover(deltaTime);
		}
	} else {
		isHover = false;
		isPressed = false;
		//afterHover();
		if (timeHovering > 0) {
			timeHovering -= deltaTime;
			isOpen = false;
			if (timeHovering < 0) {
				timeHovering = timePerFrame;
				--currentFrameIndex;
				if (currentFrameIndex < 0) {
					currentFrameIndex = 0;
				} else
					adjustPosition(currentFrameIndex + 1);
			}
		}
	}


	if (isPressed && !mouse->leftButton()) {
		isClicked = true;
		isPressed = false;

		onClick();
	} else {
		isClicked = false;
		if (!isHover) {
			isPressed = false;
		} else if (isOpen && mouse->pressed()) {
			isPressed = true;
			onPress();
		}
	}
}


void AnimatedButton::draw(SpriteBatch* batch) {

	batch->Draw(animation->texture.Get(), position,
		&animation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);
}

void AnimatedButton::adjustPosition(int lastFrame) {

	Vector2 oldPos = position;
	Vector2 newPos = oldPos;

	Vector2 oldSize = Vector2(
		animation->animationFrames[lastFrame]->sourceRect.right
		- animation->animationFrames[lastFrame]->sourceRect.left,
		animation->animationFrames[lastFrame]->sourceRect.bottom
		- animation->animationFrames[lastFrame]->sourceRect.top);

	Vector2 newSize = Vector2(getWidth(), getHeight());
	Vector2 difference = newSize - oldSize;

	newPos -= difference / 2;
	setPosition(newPos);
}


void AnimatedButton::setFont(const pugi::char_t* font) {
}

void AnimatedButton::setText(wstring text) {
}


/** Not used in Animated Button */
const Vector2& XM_CALLCONV AnimatedButton::measureString() const {
	return Vector2::Zero;
}

const Vector2& AnimatedButton::getPosition() const {
	return position;
}

const int AnimatedButton::getWidth() const {
	return animation->animationFrames[currentFrameIndex]->sourceRect.right
		- animation->animationFrames[currentFrameIndex]->sourceRect.left;
}

const int AnimatedButton::getHeight() const {
	return animation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- animation->animationFrames[currentFrameIndex]->sourceRect.top;
}

bool AnimatedButton::clicked() {
	return isClicked;
}

bool AnimatedButton::pressed() {
	return isPressed;
}

bool AnimatedButton::hovering() {
	return isHover;
}

void AnimatedButton::setToUnpressedState() {
}

void AnimatedButton::setToHoverState() {
}

void AnimatedButton::setToSelectedState() {
}
