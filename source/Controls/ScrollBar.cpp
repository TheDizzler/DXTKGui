#include "ScrollBar.h"


/** **** SCROLLBAR **** **/
ScrollBar::ScrollBar(Vector2 pos) {

	position = pos;
}

ScrollBar::~ScrollBar() {
}

#include "../Controls/GUIFactory.h"
bool ScrollBar::initialize(ComPtr<ID3D11ShaderResourceView> pixelTexture,
	size_t maxHght) {

	maxHeight = maxHght;

	scrollBarDownButton.reset((ImageButton*)
		guiFactory->createImageButton("ScrollBar Down",
			"ScrollBar Down Pressed"));
	scrollBarDownButton->setPosition(
		Vector2(position.x - scrollBarDownButton->getWidth(),
			position.y + maxHeight - scrollBarDownButton->getHeight()));

	scrollBarDownButton->action = Button::DOWN;


	scrollBarUpButton.reset((ImageButton*)
		guiFactory->createImageButton("ScrollBar Up",
			"ScrollBar Up Pressed"));
	scrollBarUpButton->setPosition(
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y));

	scrollBarUpButton->action = Button::UP;

	// use this for HitArea
	scrollBarPosition =
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y + scrollBarUpButton->getHeight());
	scrollBarRect.left = 0;
	scrollBarRect.top = 0;
	scrollBarRect.bottom = maxHeight
		- scrollBarUpButton->getHeight() * 2;
	scrollBarRect.right = scrollBarUpButton->getWidth();

	hitArea.reset(new HitArea(
		Vector2::Zero, Vector2(scrollBarRect.right, scrollBarRect.bottom)));


	pixel = pixelTexture;

	Vector2 scrubberStartPos(
		scrollBarPosition.x,
		scrollBarPosition.y);

	scrubber.reset(new Scrubber(pixel));
	scrubber->setDimensions(scrubberStartPos,
		Vector2(scrollBarRect.right, scrollBarRect.bottom),
		scrollBarRect.bottom);

	return true;
}

void ScrollBar::setPosition(const Vector2 newPosition) {

	position = newPosition;

	scrollBarDownButton->setPosition(
		Vector2(newPosition.x - scrollBarDownButton->getWidth(),
			newPosition.y + maxHeight - scrollBarDownButton->getHeight()));

	scrollBarUpButton->setPosition(
		Vector2(newPosition.x - scrollBarUpButton->getWidth(),
			newPosition.y));

	scrollBarPosition =
		Vector2(newPosition.x - scrollBarUpButton->getWidth(),
			newPosition.y + scrollBarUpButton->getHeight());

	Vector2 scrubberPos = scrubber->getPosition();
	scrubberPos.x = newPosition.x;
	scrubber->setPosition(scrubberPos);

	hitArea->position = scrollBarPosition;
}


void ScrollBar::setScrollBar(int totalItems, int itemHeight, int maxDisplayItems) {

	percentForOneItem = (double) 1 / (totalItems - maxDisplayItems);
	double percentToShow;
	if (totalItems < maxDisplayItems) {
		percentToShow = 1;
		percentForOneItem = 0;
		scrollBarRect.bottom = (totalItems * itemHeight)
			- scrollBarUpButton->getHeight() * 2;

		Vector2 newButtonPos = scrollBarDownButton->getPosition();
		newButtonPos.y = scrollBarPosition.y + scrollBarRect.bottom
			+ scrollBarDownButton->getHeight() / 2;
		scrollBarDownButton->setPosition(newButtonPos);

	} else {
		percentToShow = percentForOneItem * maxDisplayItems;
		scrollBarRect.bottom = maxHeight
			- scrollBarUpButton->getHeight() * 2;
	}
	/*wostringstream ws;
	ws << "\n" << "percentForOneItem: " << percentForOneItem;
	ws << " increment: " << increment;
	OutputDebugString(ws.str().c_str());*/


	scrubber->setDimensions(
		Vector2(scrollBarPosition.x, scrollBarPosition.y),
		Vector2(scrollBarRect.right, scrollBarRect.bottom * percentToShow),
		scrollBarRect.bottom);
}


void ScrollBar::update(double deltaTime, MouseController* mouse) {

	isHover = hitArea->contains(mouse->getPosition());
	// update scrubber
	scrubber->update(deltaTime, mouse);

	if (!scrubber->hovering() && isHover
		&& mouse->leftButtonDown()) {

		if (firstClickTimer == 0) {
			bool up = mouse->getPosition().y > scrubber->getPosition().y;
			scrubber->scroll(percentForOneItem * (up ? 5 : -5));
		}

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			bool up = mouse->getPosition().y > scrubber->getPosition().y;
			scrubber->scroll(percentForOneItem* (up ? 5 : -5));
			firstClickTimer = autoScrollDelay;
		}
		//scrubber->setScrollPositionByCoord(mouse->getPosition().y);
	}

	scrollBarDownButton->update(deltaTime, mouse);
	scrollBarUpButton->update(deltaTime, mouse);
	if (scrollBarDownButton->selected()) {
		// scroll down
		if (firstClickTimer == 0)
			scrubber->scroll(percentForOneItem);

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			scrubber->scroll(percentForOneItem);
			firstClickTimer = autoScrollDelay;
		}
	} else if (scrollBarUpButton->selected()) {
		// scroll up
		if (firstClickTimer == 0)
			scrubber->scroll(-percentForOneItem);

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			scrubber->scroll(-percentForOneItem);
			firstClickTimer = autoScrollDelay;
		}
	}

	if (scrollBarDownButton->clicked()
		|| scrollBarUpButton->clicked()
		|| !(scrollBarDownButton->hovering()
			|| scrollBarUpButton->hovering()
			|| isHover))
		firstClickTimer = 0;

	percentScroll = scrubber->percentAt;
}


void ScrollBar::draw(SpriteBatch * batch) {

	scrollBarDownButton->draw(batch);
	scrollBarUpButton->draw(batch);

	// draw bar
	batch->Draw(pixel.Get(), scrollBarPosition, &scrollBarRect,
		::DirectX::Colors::Gray.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// draw scrubber
	scrubber->draw(batch);

}

void ScrollBar::setScrollPositionByPercent(float newPositionPercentage) {
	percentScroll = newPositionPercentage;
	scrubber->setScrollPositionByPercent(newPositionPercentage);
}

void ScrollBar::scrollByIncrement(int scrollIncrement) {
	scrubber->scroll(scrollIncrement * percentForOneItem);
}

const Vector2& ScrollBar::getPosition() const {
	return position;
}

const int ScrollBar::getWidth() const {
	return scrollBarDownButton->getWidth();
}

const int ScrollBar::getHeight() const {
	return maxHeight;
}

bool ScrollBar::clicked() {
	return isClicked;
}

bool ScrollBar::selected() {
	return isSelected;
}

bool ScrollBar::hovering() {
	return isHover;
}

/* Unused in ScrollBar. */
void ScrollBar::setFont(const pugi::char_t* font) {
}
/* Unused in ScrollBar. */
void ScrollBar::setText(wstring text) {
}
/* Unused in ScrollBar. */
XMVECTOR XM_CALLCONV ScrollBar::measureString() const {
	return Vector2();
}
/** **** ScrollBar END **** **/




/** **** Scrubber **** **/
Scrubber::Scrubber(ComPtr<ID3D11ShaderResourceView> pixel)
	: RectangleSprite(pixel) {
}

Scrubber::~Scrubber() {
}

void Scrubber::setDimensions(const Vector2& startPos, const Vector2& size,
	const int scrllBrHght) {

	width = size.x;
	height = size.y;

	minPosition = startPos;

	scrollBarHeight = scrllBrHght;

	maxPosition = startPos;
	maxPosition.y += scrollBarHeight - height;

	position = minPosition;


	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	hitArea.reset(new HitArea(minPosition,
		Vector2(width*scale.x, height*scale.y)));

	minMaxDifference = maxPosition.y - minPosition.y;
}


void Scrubber::update(double deltaTime, MouseController* mouse) {

	isHover = hitArea->contains(mouse->getPosition());

	if (isHover && mouse->leftButtonDown() && !mouse->leftButtonLastDown()) {
		isPressed = true;
		pressedPosition = mouse->getPosition().y - position.y;

	} else if (!mouse->leftButtonDown())
		isPressed = false;

	if (isPressed) {
		tint = selectedColor;
		if (minMaxDifference == 0)
			return;
		setScrollPositionByCoord(mouse->getPosition().y - pressedPosition);

	} else if (isHover)
		tint = hoverColor;
	else
		tint = normalColor;
}


void Scrubber::setScrollPositionByCoord(int newCoordinatePosition) {

	position.y = newCoordinatePosition;

	if (position.y < minPosition.y)
		position.y = minPosition.y;
	else if (position.y > maxPosition.y)
		position.y = maxPosition.y;

	hitArea->position.y = position.y;

	double distanceBetweenPosAndMax = maxPosition.y - position.y;

	percentAt = (minMaxDifference - distanceBetweenPosAndMax)
		/ (minMaxDifference);

	/*wostringstream ws;
	ws << "percentAt: " << percentAt;
	ws << " distanceBetweenPosAndMax: " << distanceBetweenPosAndMax;
	ws << " position.y: " << position.y;
	ws << " maxPosition.y: " << maxPosition.y << "\n";
	OutputDebugString(ws.str().c_str());*/
}

void Scrubber::setScrollPositionByPercent(float newPositionPercentage) {

	percentAt = newPositionPercentage;
	position.y = (minMaxDifference * percentAt) + minPosition.y;
	hitArea->position = Vector2(position.x, position.y);
	hitArea->size = Vector2(width*scale.x, height*scale.y);
}


void Scrubber::scroll(double increment) {

	percentAt += increment;
	if (percentAt < 0)
		percentAt = 0;
	else if (percentAt > 1)
		percentAt = 1;

	position.y = (minMaxDifference * percentAt) + minPosition.y;
	hitArea->position.y = position.y;
}


bool Scrubber::hovering() {
	return isHover;
}

bool Scrubber::pressed() {
	return isPressed;
}
/** **** Scrubber END **** **/