#include "Button.h"

#include "../GUIFactory.h"
Button::Button(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	const pugi::char_t* font) : GUIControl(factory, mouseController) {


	hitArea = make_unique<HitArea>();

	position = Vector2(-1, -1);

	buttonLabel.reset(guiFactory->createTextLabel(Vector2::Zero, L"", font, true));
	rectSprite.reset(guiFactory->createRectangle());
	frame.reset(guiFactory->createRectangleFrame());
	texturePanel.reset(guiFactory->createPanel());
}


Button::~Button() {
	if (actionListener != NULL)
		delete actionListener;
}


const int textMargin = 10;

void Button::setDimensions(const Vector2& pos, const Vector2& size,
	const int frmThcknss) {

	frameThickness = frmThcknss;

	Vector2 labelSize = measureString();
	Vector2 newSize = size;


	if ((labelSize.x + textMargin * 2) > size.x) {
		newSize.x = labelSize.x + textMargin * 2;
	}
	if ((labelSize.y + textMargin * 2) > size.y) {
		newSize.y = labelSize.y + textMargin * 2;
	}

	hitArea->size = newSize;
	projectedHitArea->size = newSize;
	width = newSize.x;
	height = newSize.y;

	frame->setDimensions(position, hitArea->size);
	rectSprite->setDimensions(position, hitArea->size);

	setPosition(pos);
	setLayerDepth(.9);

	setToUnpressedState();

	refreshTexture = true;
}


bool Button::update(double deltaTime) {

	updateProjectedHitArea();
	if (projectedHitArea->contains(mouse->getPosition())) {
		isHover = true;
		if (!isPressed) {
			if (!hasBeenSetHover) {
				onHover();
				setToHoverState();
				hasBeenSetHover = true;
				hasBeenSetUnpressed = false;
				refreshTexture = true;
			}
		}
	} else
		isHover = false;

	if (isPressed && !mouse->leftButton()) {
		isClicked = true;
		isPressed = false;
		onClick();
		setToUnpressedState();
		hasBeenSetUnpressed = false;
		hasBeenSetHover = false;
		refreshTexture = true;
	} else {
		isClicked = false;
		if (!isHover) {
			if (!hasBeenSetUnpressed) {
				isPressed = false;
				setToUnpressedState();
				hasBeenSetUnpressed = true;
				hasBeenSetHover = false;
				refreshTexture = true;
			}
		} else if (mouse->pressed()) {
			isPressed = true;
			setToSelectedState();
			hasBeenSetUnpressed = false;
			hasBeenSetHover = false;
			refreshTexture = true;
		}
	}

	if (buttonLabel->update(deltaTime))
		refreshTexture = true;

	if (frame.get()) {
		if (frame->update())
			refreshTexture = true;
	}

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}

	return false;
}


void Button::draw(SpriteBatch* batch) {

	texturePanel->draw(batch);
	/*rectSprite->draw(batch);
	frame->draw(batch);
	buttonLabel->draw(batch);*/

}


unique_ptr<GraphicsAsset> Button::texturize() {
	return guiFactory->createTextureFromIElement2D(this);
}

void Button::textureDraw(SpriteBatch * batch, ComPtr<ID3D11Device> device) {

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

	Vector2 labelSize = measureString();

	if ((labelSize.x + textMargin * 2) > hitArea->size.x) {
		hitArea->size.x = labelSize.x + textMargin * 2;
	}
	if ((labelSize.y + textMargin * 2) > hitArea->size.y) {
		hitArea->size.y = labelSize.y + textMargin * 2;
	}

	projectedHitArea->size = hitArea->size;
	width = hitArea->size.x;
	height = hitArea->size.y;

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


void Button::moveBy(const Vector2& moveVector) {
	GUIControl::moveBy(moveVector);
	texturePanel->moveBy(position);
}


void Button::setPosition(const Vector2& pos) {

	GUIControl::setPosition(pos);

	buttonLabel->setPosition(position);
	if (frame != NULL)
		frame->setPosition(position);
	if (rectSprite != NULL)
		rectSprite->setDimensions(position, hitArea->size);

	// center text
	positionText();

	texturePanel->setPosition(position);
}

void Button::positionText() {

	Vector2 textsize = buttonLabel->measureString();
	if (textsize.x > 0) {

		Vector2 newPos;
		//if (resized) {
		//	newPos = Vector2(
		//		position.x + (getScaledWidth() - textsize.x) / 2 ,
		//		position.y + (getScaledHeight() - textsize.y) / 2 /*+ textMargin*/);
		//	resized = false;
		//} else
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

void Button::setLayerDepth(float newDepth, bool frontToBack) {

	layerDepth = newDepth - .00001;
	if (layerDepth < 0) {
		if (!frontToBack)
			layerDepth = .00001;
		else
			layerDepth = 0;
	}
	float nudge = .00000001;
	if (!frontToBack)
		nudge *= -1;
	rectSprite->setLayerDepth(layerDepth + nudge, frontToBack);
	buttonLabel->setLayerDepth(layerDepth + nudge * 2, frontToBack);
	frame->setLayerDepth(layerDepth + nudge * 3, frontToBack);
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

void Button::setFont(const pugi::char_t* font) {

	buttonLabel->setFont(guiFactory->getFont(font));
	positionText();
}



/** **** ImageButton **** **/
ImageButton::ImageButton(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	unique_ptr<Sprite> buttonSprite, const pugi::char_t* font)
	: Button(factory, mouseController, font) {

	// a rough guesstimate
	setTextOffset(Vector2(0, -5), Vector2(0, 0));

	normalSprite = move(buttonSprite);

	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());
	width = size.x;
	height = size.y;
	hitArea.reset(new HitArea(Vector2::Zero, size));

	setToUnpressedState();
}


ImageButton::ImageButton(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	unique_ptr<Sprite> upButtonSprite, unique_ptr<Sprite> downButtonSprite,
	const pugi::char_t* font)
	: Button(factory, mouseController, font) {

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
	//OutputDebugString(L"\n\n*** ImageButton Release ***\n\t =>");
	//texture = NULL;
	//normalSprite.reset();
	//pressedSprite.reset();
	//OutputDebugString(L"*** ImageButton Done ***\n");
}


void ImageButton::draw(SpriteBatch* batch) {

	batch->Draw(texture, normalSprite->getPosition(), &normalSprite->getRect(),
		tint, rotation, normalSprite->getOrigin(), scale, SpriteEffects_None, layerDepth);
	buttonLabel->draw(batch);
}

void ImageButton::setDimensions(const Vector2& pos, const Vector2& size) {

	setScale(Vector2(size.x / getWidth(), size.y / getHeight()));
	Vector2 newpos = pos;
	setPosition(newpos);
}

void ImageButton::setText(wstring text) {
	buttonLabel->setText(text);
	positionText();
}

void ImageButton::setPosition(const Vector2& pos) {

	Button::setPosition(pos);
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

void ImageButton::setLayerDepth(float newDepth, bool frontToBack) {

	layerDepth = newDepth;
	float nudge = .00000001;
	if (!frontToBack)
		nudge *= -1;
	buttonLabel->setLayerDepth(newDepth + nudge, frontToBack);
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
AnimatedButton::AnimatedButton(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	shared_ptr<Animation> anim, Vector2 pos) : GUIControl(factory, mouseController) {

	animation = anim;

	if (animation->animationFrames.size() > 0)
		currentFrameIndex = 0;

	position = pos;
	hitArea.reset(new HitArea(position, Vector2(getWidth(), getHeight())));
	center = Vector2(getWidth() / 2, getHeight() / 2);
}

AnimatedButton::~AnimatedButton() {
	if (actionListener != NULL)
		delete actionListener;
}


bool AnimatedButton::update(double deltaTime) {

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

	return false;
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

void AnimatedButton::setLayerDepth(float newDepth, bool frontToBack) {
	layerDepth = newDepth;
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
