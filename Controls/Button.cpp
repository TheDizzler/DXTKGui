#include "Button.h"
#include "../GUIFactory.h"


Button::Button(GUIFactory* factory, MouseController* mouseController,
	const pugi::char_t* font) : Selectable(factory, mouseController) {

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

void Button::reloadGraphicsAsset() {

	buttonLabel->reloadGraphicsAsset();
	rectSprite->reloadGraphicsAsset(guiFactory);
	frame->reloadGraphicsAsset();
	texturePanel.reset(guiFactory->createPanel());

	refreshTexture = true;
}


const int textMargin = 10;

void Button::setDimensions(const Vector2& pos, const Vector2& size,
	const int frmThcknss) {

	frameThickness = frmThcknss;

	Vector2 labelSize = measureString();
	Vector2 newSize = size;


	if ((labelSize.x + (textMargin + frameThickness) * 2) > size.x) {
		newSize.x = labelSize.x + (textMargin + frameThickness) * 2;
	}
	if ((labelSize.y + (textMargin + frameThickness) * 2) > size.y) {
		newSize.y = labelSize.y + (textMargin + frameThickness) * 2;
	}

	hitArea.size = newSize;
	projectedHitArea.size = newSize;
	width = newSize.x;
	height = newSize.y;

	frame->setDimensions(position, hitArea.size, frameThickness);
	rectSprite->setDimensions(position, hitArea.size);

	setPosition(pos);
	setLayerDepth(layerDepth);

	setToUnpressedState();
	refreshTexture = true;
}


bool Button::updateSelect(double deltaTime) {

	updateProjectedHitArea();
	if (projectedHitArea.contains(mouse->getPosition())) {
		mouseHover = true;
		if (!isPressed) {
			if (!hasBeenSetHover) {
				onHover();
			}
		}
	} else if (mouseHover) {
		mouseHover = false;
	}

	if (isPressed && mouse->clicked()) {
		onClick();
	} else {
		isClicked = false;
		if (!isHover) {
			if (!hasBeenSetUnpressed) {
				resetState();
			}
		} else if (mouseHover && mouse->pressed()) {
			onPress();
		}
	}

	if (buttonLabel->update(deltaTime) && !isLetterJammer)
		refreshTexture = true;

	if (frame->update())
		refreshTexture = true;

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}

	return false;
}

bool Button::update(double deltaTime) {

	updateProjectedHitArea();
	if (projectedHitArea.contains(mouse->getPosition())) {
		if (!mouseHover && isPressed) {
			//OutputDebugString(L"Reentry!\n");
			onHover();
		}
		mouseHover = true;
		if (!isPressed) {
			if (!hasBeenSetHover) {
				onHover();
			}
		}
	} else if (mouseHover) {
		isHover = false;
		mouseHover = false;
		//hasBeenReset = false;
	} else if (!hasBeenReset && !isHover) {
		//OutputDebugString(L"Reset STate!\n");
		resetState();
		hasBeenReset = true;
	} else if (!mouse->leftButton()) {
			isPressed = false;
	}

	if (isPressed && mouseHover && mouse->clicked()) {
		onClick();
	} else {
		isClicked = false;
		if (!isHover) {
			if (!hasBeenSetUnpressed) {
				if (isPressed) {
					//OutputDebugString(L"partial reset!\n");
					isHover = false;
					mouseHover = false;
					setToUnpressedState();
					hasBeenSetHover = false;
					refreshTexture = true;
				} else {
					//resetState();
					hasBeenReset = false;
				}
			}
		} else if (mouseHover && mouse->pressed()) {
			onPress();
		}
	}

	if (buttonLabel->update(deltaTime) && !isLetterJammer)
		refreshTexture = true;

	if (frame->update())
		refreshTexture = true;

	if (refreshTexture) {
		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}

	return false;
}

void Button::draw(SpriteBatch* batch) {

	texturePanel->draw(batch);
	if (isLetterJammer)
		buttonLabel->draw(batch);

}

unique_ptr<GraphicsAsset> Button::texturize() {
	return guiFactory->createTextureFromTexturizable(this);
}

void Button::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {

	rectSprite->draw(batch);
	frame->draw(batch);
	if (!isLetterJammer)
		buttonLabel->draw(batch);
}


void Button::setTextLabel(TextLabel* newLabel, bool letterJammer) {

	buttonLabel.reset(newLabel);
	positionText();
	isLetterJammer = letterJammer;
}


void Button::setToUnpressedState() {

	rectSprite->setTint(normalColor);
	buttonLabel->setTint(normalColorText);
	buttonLabel->setPosition(unpressedTextPosition);

	hasBeenSetUnpressed = true;
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
		setDimensions(position, hitArea.size, frame->getThickness());
	else {
		Vector2 labelSize = measureString();

		if ((labelSize.x + (textMargin + frameThickness) * 2) > hitArea.size.x) {
			hitArea.size.x = labelSize.x + (textMargin + frameThickness) * 2;
		}
		if ((labelSize.y + (textMargin + frameThickness) * 2) > hitArea.size.y) {
			hitArea.size.y = labelSize.y + (textMargin + frameThickness) * 2;
		}

		projectedHitArea.size = hitArea.size;
		width = hitArea.size.x;
		height = hitArea.size.y;
	}

}

const wchar_t* Button::getText() {
	return buttonLabel->getText();
}

const Vector2 XM_CALLCONV Button::measureString() const {
	return buttonLabel->measureString();
}


void Button::setTextOffset(const Vector2& unpressedOffset,
	const Vector2& pressedOffset) {

	unpressedTextOffset = unpressedOffset;
	pressedTextOffset = pressedOffset;
}


void Button::moveBy(const Vector2& moveVector) {
	GUIControl::moveBy(moveVector);

	// really not sure why this is not needed... 
	/*if (frame != NULL)
	frame->moveBy(moveVector);*/
	/*if (rectSprite != NULL)
	rectSprite->moveBy(position);*/
	//positionText();

	//texturePanel->moveBy(position);
}


void Button::setPosition(const Vector2& pos) {
	Vector2 oldpos = position;
	GUIControl::setPosition(pos);

	if (oldpos == Vector2(-1, -1))
		setDimensions(position, hitArea.size, frame->getThickness());

	frame->setPosition(position);
	rectSprite->setDimensions(position, hitArea.size);

	positionText();

	texturePanel->setPosition(position);
}

void Button::setOrigin(const Vector2& org) {
	origin = org;
	texturePanel->setOrigin(org);
}

void Button::setRotation(const float rot) {
	rotation = rot;
	texturePanel->setRotation(rotation);
}

void Button::setActionListener(ActionListener* iOnC) {
	if (actionListener != NULL)
		delete actionListener;
	onClickFunction = &ActionListener::onClick;
	onHoverFunction = &ActionListener::onHover;
	onPressFunction = &ActionListener::onPress;
	onResetFunction = &ActionListener::resetState;
	actionListener = iOnC;
}

void Button::onClick() {
	isClicked = true;
	if (actionListener != NULL) {
		isClicked = isPressed = false;
		(actionListener->*onClickFunction)(this);
	}
	resetState();
	hasBeenSetUnpressed = false;
}

void Button::onPress() {
	isPressed = true;
	if (actionListener != NULL) {
		(actionListener->*onPressFunction)(this);
	}
	setToSelectedState();
	hasBeenSetUnpressed = false;
	hasBeenSetHover = false;
	refreshTexture = true;
}

void Button::onHover() {
	isHover = true;
	if (actionListener != NULL) {
		(actionListener->*onHoverFunction)(this);
	}
	setToHoverState();
	hasBeenSetHover = true;
	hasBeenSetUnpressed = false;
	refreshTexture = true;
}

void Button::resetState() {
	if (actionListener != NULL) {
		(actionListener->*onResetFunction)(this);
	}
	isHover = false;
	mouseHover = false;
	isPressed = false;
	setToUnpressedState();
	hasBeenSetHover = false;
	refreshTexture = true;
}

void Button::positionText() {

	Vector2 textsize = buttonLabel->measureString();
	if (textsize.x > 0) {

		Vector2 newPos;
		newPos = Vector2(
			(position.x) + (getScaledWidth() - textsize.x) / 2,
			(position.y) + (getScaledHeight() - textsize.y) / 2);

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
	return hitArea.size.x;
}

const int Button::getScaledHeight() const {
	return hitArea.size.y;
}

void Button::setUnpressedColor(const Color& newColor) {
	normalColor = newColor;
	rectSprite->setTint(normalColor);
}

void Button::setPressedColor(const Color& newColor) {
	selectedColor = newColor;
}

void Button::setHoverColor(const Color& newColor) {
	hoverColor = newColor;
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
ImageButton::ImageButton(GUIFactory* factory, MouseController* mouseController,
	unique_ptr<Sprite> buttonSprite, const pugi::char_t* font)
	: Button(factory, mouseController, font) {

	// a rough guesstimate
	setTextOffset(Vector2(0, -5), Vector2(0, 0));

	normalSprite = move(buttonSprite);

	Vector2 size = Vector2(normalSprite->getWidth(), normalSprite->getHeight());
	width = size.x;
	height = size.y;
	hitArea.size = size;

	setToUnpressedState();
}


ImageButton::ImageButton(GUIFactory* factory, MouseController* mouseController,
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
	hitArea.size = size;

	setToUnpressedState();
}


ImageButton::~ImageButton() {
}

void ImageButton::reloadGraphicsAsset() {
	buttonLabel->reloadGraphicsAsset();

	texture = NULL;
	normalSprite->reloadGraphicsAsset(guiFactory);
	if (pressedSprite != NULL)
		pressedSprite->reloadGraphicsAsset(guiFactory);
	texture = normalSprite->getTexture().Get();

	rectSprite->reloadGraphicsAsset(guiFactory);
	frame->reloadGraphicsAsset();

	texturePanel.reset(guiFactory->createPanel());
	refreshTexture = true;
}


void ImageButton::draw(SpriteBatch* batch) {

	texturePanel->draw(batch);
}

void ImageButton::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {

	batch->Draw(texture, normalSprite->getPosition(), &sourceRect,
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

void ImageButton::moveBy(const Vector2& moveVector) {
	GUIControl::moveBy(moveVector);
	normalSprite->moveBy(moveVector);
	texturePanel->setPosition(position);
}

void ImageButton::setPosition(const Vector2& pos) {
	GUIControl::setPosition(pos);
	Vector2 spritePos = position;
	spritePos.x += normalSprite->getWidth() / 2;
	spritePos.y += normalSprite->getHeight() / 2;
	normalSprite->setPosition(spritePos);

	positionText();

	texturePanel->setPosition(position);
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
	sourceRect = normalSprite->getRect();

	hasBeenSetUnpressed = true;
}

void ImageButton::setToHoverState() {

	buttonLabel->setTint(hoverColorText);
	tint = hoverColor;
	texture = normalSprite->getTexture().Get();
	sourceRect = normalSprite->getRect();
}

void ImageButton::setToSelectedState() {

	buttonLabel->setPosition(pressedTextPosition);
	tint = normalColor;
	if (pressedSprite.get() != NULL) {
		texture = pressedSprite->getTexture().Get();
		sourceRect = pressedSprite->getRect();
	} else
		tint = selectedColor;
}
/** ***** END OF IMAGE BUTTON **** **/



/** ***** Animated Button ***** **/
AnimatedButton::AnimatedButton(GUIFactory* factory, MouseController* mouseController,
	Animation* anim, Vector2 pos) : Selectable(factory, mouseController) {

	animation = anim;

	if (animation->animationFrames.size() > 0)
		currentFrameIndex = 0;

	position = pos;
	hitArea.position = position;
	hitArea.size = Vector2(getWidth(), getHeight());
	center = Vector2(getWidth() / 2, getHeight() / 2);
}

AnimatedButton::~AnimatedButton() {
	if (actionListener != NULL)
		delete actionListener;
}

void AnimatedButton::reloadGraphicsAsset() {
	string name = animation->animationName;
	animation = guiFactory->getAnimation(name.c_str());
}


bool AnimatedButton::updateSelect(double deltaTime) {
	return update(deltaTime);
}

bool AnimatedButton::update(double deltaTime) {

	if (hitArea.contains(mouse->getPosition())) {
		//lastWasHover = true;
		if (!isPressed) {
			timeHovering += deltaTime;
			onHover();
		}
	} else/* if (lastWasHover)*/ {
		//lastWasHover = false;
		isHover = false;
		isPressed = false;
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
const Vector2 XM_CALLCONV AnimatedButton::measureString() const {
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

void AnimatedButton::setActionListener(ActionListener* iOnC) {
	if (actionListener != NULL)
		delete actionListener;
	onClickFunction = &ActionListener::onClick;
	onHoverFunction = &ActionListener::onHover;
	onPressFunction = &ActionListener::onPress;
	actionListener = iOnC;
}

void AnimatedButton::onClick() {
	if (actionListener != NULL) {
		(actionListener->*onClickFunction)(this);
	} else {
		currentFrameIndex = animation->animationFrames.size() - 1;
	}

	isClicked = isPressed = false;
}

void AnimatedButton::onPress() {
	if (actionListener != NULL) {
		(actionListener->*onPressFunction)(this);
	} else
		currentFrameIndex = animation->animationFrames.size() - 2;
}

void AnimatedButton::onHover() {
	isHover = true;
	if (actionListener != NULL) {
		(actionListener->*onHoverFunction)(this);
	} else {
		if (timeHovering > timePerFrame) {
			timeHovering = 0;
			++currentFrameIndex;
			if (currentFrameIndex > animation->animationFrames.size() - 3) {
				currentFrameIndex = animation->animationFrames.size() - 3;
				isOpen = true;
			} else
				adjustPosition(currentFrameIndex - 1);
		}
	}
}

void AnimatedButton::resetState() {
	setToUnpressedState();
}
