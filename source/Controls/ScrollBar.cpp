#include "ScrollBar.h"


/** **** SCROLLBAR **** **/
ScrollBar::ScrollBar(Vector2 pos) {

	position = pos;
}

ScrollBar::~ScrollBar() {
}

#include "../Controls/GUIFactory.h"
bool ScrollBar::initialize(GraphicsAsset* const pixelAsset,
	size_t barHght, ImageButton* scrollButtons[2], unique_ptr<Sprite> scrllBrTrck,
	GraphicsAsset* scrbbr) {

	//pixel = pixelAsset->getTexture();
	barHeight = barHght;

	if (scrollButtons == NULL || scrollButtons[0] == NULL) {

		scrollBarUpButton.reset((ImageButton*)
			guiFactory->createImageButton("ScrollBar Up",
				"ScrollBar Up Pressed"));

	} else {
		scrollBarUpButton.reset(scrollButtons[0]);
	}

	scrollBarUpButton->setPosition(
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y));

	if (scrollButtons == NULL || scrollButtons[1] == NULL) {

		scrollBarDownButton.reset((ImageButton*)
			guiFactory->createImageButton("ScrollBar Down",
				"ScrollBar Down Pressed"));

	} else {
		scrollBarDownButton.reset(scrollButtons[1]);
	}

	scrollBarDownButton->setPosition(
		Vector2(position.x - scrollBarDownButton->getWidth(),
			position.y + barHeight - scrollBarDownButton->getHeight()));


	// use this for HitArea
	Vector2 scrollBarPosition =
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y + scrollBarUpButton->getHeight());
	Vector2 trackSize = Vector2(scrollBarUpButton->getWidth(),
		barHeight - scrollBarUpButton->getHeight() * 2);

	if (scrllBrTrck.get() == NULL) {
		scrollBarTrack.reset(new RectangleSprite(pixelAsset->getTexture(), scrollBarPosition,
			trackSize));
		scrollBarTrack->setTint(Colors::Gray.v);
	} else {
		scrollBarTrack = move(scrllBrTrck);
		scrollBarTrack->setOrigin(Vector2(0, 0));
		scrollBarTrack->setDimensions(scrollBarPosition, trackSize);
	}

	Vector2 scrubberStartPos(
		scrollBarPosition.x,
		scrollBarPosition.y);

	if (scrbbr == NULL) {
		scrubber.reset(new Scrubber(pixelAsset, false));

	} else {
		scrubber.reset(new Scrubber(scrbbr, true));
	}
	scrubber->setDimensions(scrubberStartPos, trackSize, trackSize.y);
	return true;
}


void ScrollBar::setPosition(const Vector2 newPosition) {

	position = newPosition;

	scrollBarDownButton->setPosition(
		Vector2(newPosition.x - scrollBarDownButton->getWidth(),
			newPosition.y + barHeight - scrollBarDownButton->getHeight()));

	scrollBarUpButton->setPosition(
		Vector2(newPosition.x - scrollBarUpButton->getWidth(),
			newPosition.y));

	//scrollBarPosition =
	scrollBarTrack->setPosition(
		Vector2(newPosition.x - scrollBarUpButton->getWidth(),
			newPosition.y + scrollBarUpButton->getHeight()));

	Vector2 scrubberPos = scrubber->getPosition();
	scrubberPos.x = newPosition.x;
	scrubber->setPosition(scrubberPos);

	//hitArea->position = scrollBarPosition;
}


void ScrollBar::setScrollBar(int totalItems, int itemHeight, int maxDisplayItems) {

	percentForOneItem = (double) 1 / (totalItems - maxDisplayItems);
	wostringstream ws;
	ws << "percentForOneItem: " << percentForOneItem << "\n";
	OutputDebugString(ws.str().c_str());

	double percentToShow;
	if (totalItems < maxDisplayItems) {
		percentToShow = 1;
		percentForOneItem = 0;
		//scrollBarRect.bottom = 
		scrollBarTrack->setSize(Vector2(scrollBarTrack->getWidth(),
			(totalItems * itemHeight) - scrollBarUpButton->getHeight() * 2));

		Vector2 newButtonPos = scrollBarDownButton->getPosition();
		newButtonPos.y =
			//scrollBarPosition.y + scrollBarRect.bottom
			scrollBarTrack->getPosition().y + scrollBarTrack->getHeight()
			+ scrollBarDownButton->getHeight() / 2;
		scrollBarDownButton->setPosition(newButtonPos);

	} else {
		percentToShow = percentForOneItem * maxDisplayItems;
		//scrollBarRect.bottom =
		scrollBarTrack->setSize(Vector2(scrollBarTrack->getWidth(),
			barHeight - scrollBarUpButton->getHeight() * 2));
	}



	scrubber->setDimensions(
		//Vector2(scrollBarPosition.x, scrollBarPosition.y),
		//Vector2(scrollBarRect.right, scrollBarRect.bottom * percentToShow),
		//scrollBarRect.bottom);
		scrollBarTrack->getPosition(),
		Vector2(scrollBarTrack->getWidth(), scrollBarTrack->getHeight()* percentToShow),
		scrollBarTrack->getHeight());


}


void ScrollBar::update(double deltaTime, MouseController* mouse) {

	//isHover = hitArea->contains(mouse->getPosition());
	isHover = scrollBarTrack->getHitArea()->contains(mouse->getPosition());
	// update scrubber
	scrubber->update(deltaTime, mouse);

	if (!scrubber->hovering() && isHover && mouse->leftButtonDown()) {

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

	// draw track bar
	scrollBarTrack->draw(batch);

	scrollBarDownButton->draw(batch);
	scrollBarUpButton->draw(batch);

	// draw scrubber
	scrubber->draw(batch);

}

void ScrollBar::setScrollPositionByPercent(double newPositionPercentage) {
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
	return barHeight;
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
Scrubber::Scrubber(GraphicsAsset* const graphicsAsset, bool isPixel)
	: RectangleSprite(graphicsAsset) {

	assetIsPixel = isPixel;
		
}


Scrubber::~Scrubber() {
}

void Scrubber::setDimensions(const Vector2& startPos, const Vector2& size,
	const int scrllBrHght) {

	minPosition = startPos;
	position = minPosition;

	scrollBarHeight = scrllBrHght;

	if (!assetIsPixel) {
		width = size.x;
		height = size.y;
	}

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;


	maxPosition = startPos;
	maxPosition.y += scrollBarHeight - height;

	hitArea.reset(new HitArea(position,
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

void Scrubber::setScrollPositionByPercent(double newPositionPercentage) {

	percentAt = newPositionPercentage;
	position.y = (minMaxDifference * percentAt) + minPosition.y;
	hitArea->position = Vector2(position.x, position.y);
	hitArea->size = Vector2(width*scale.x, height*scale.y);
}


void Scrubber::scroll(double increment) {

	percentAt += increment;
	if (percentAt < increment/2)
		percentAt = 0;
	else if (percentAt > 1 - increment)
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