#include "ScrollBar.h"


/** **** SCROLLBAR **** **/
ScrollBar::ScrollBar(const Vector2& pos) {

	position = pos;
	
}

ScrollBar::~ScrollBar() {
}

#include "GUIFactory.h"
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
		Vector2(position.x - scrollBarUpButton->getWidth(), position.y));


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
		scrollBarTrack.reset(new RectangleSprite(pixelAsset->getTexture(),
			scrollBarPosition, trackSize));
		scrollBarTrack->setTint(Color(.502, .502, .502, 1));
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
	//scrubber->setDimensions(scrubberStartPos, trackSize, trackSize.y);
	scrubber->setDimensions(scrollBarTrack.get(), 1, 0);
	return true;
}

/* newPosition.x is position of right side of scrollbar's container. */
void ScrollBar::setPosition(const Vector2& newPosition) {
	/** Do not use ScrollBar's HitArea - it is NULL. */
	position = newPosition;

	int xPos = newPosition.x - scrollBarUpButton->getWidth();
	scrollBarUpButton->setPosition(Vector2(xPos, newPosition.y));

	scrollBarDownButton->setPosition(
		Vector2(xPos,
			newPosition.y + barHeight - scrollBarUpButton->getHeight()));

	scrollBarTrack->setPosition(
		Vector2(xPos, newPosition.y + scrollBarUpButton->getHeight()));

	/*wostringstream ws;
	ws << "newPosition.y: " << newPosition.y << "\n";
	OutputDebugString(ws.str().c_str());*/

	Vector2 scrubberPos = scrubber->getPosition();
	scrubberPos.x = xPos;
	scrubber->setPosition(scrubberPos);

}

void ScrollBar::moveBy(const Vector2& moveVector) {
	
	position += moveVector;
	scrollBarUpButton->moveBy(moveVector);
	scrollBarDownButton->moveBy(moveVector);
	scrollBarTrack->moveBy(moveVector);
	scrubber->moveBy(moveVector);
}

void ScrollBar::setBarHeight(int barHght) {
	barHeight = barHght;

	scrollBarDownButton->setPosition(
		Vector2(position.x - scrollBarDownButton->getWidth(),
			position.y + barHeight - scrollBarDownButton->getHeight()));


	// use this for HitArea
	Vector2 scrollBarPosition =
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y + scrollBarUpButton->getHeight());
	Vector2 trackSize = Vector2(scrollBarUpButton->getWidth(),
		barHeight - scrollBarUpButton->getHeight() * 2);

	Vector2 scrubberStartPos(
		scrollBarPosition.x,
		scrollBarPosition.y);

	scrubber->setDimensions(scrollBarTrack.get(), 1, 0);
}


void ScrollBar::setScrollBar(int totalItems, int itemHeight, int maxDisplayItems) {

	if (totalItems == 0)
		return;
	percentForOneItem = (double) 1 / (totalItems);
	scrubberPercentForOneItem = (double) 1 / (totalItems - maxDisplayItems);
	double maxPercent;

	// % of list being displayed
	double percentShowing;
	if (totalItems <= maxDisplayItems) {
		percentShowing = 1;
		percentForOneItem = 0;
		scrubberPercentForOneItem = 1;
		maxPercent = 0;
		scrollBarTrack->setSize(Vector2(scrollBarTrack->getWidth(),
			(totalItems * itemHeight) - scrollBarUpButton->getHeight() * 2));

	} else {
		percentShowing = percentForOneItem * (maxDisplayItems);
		scrollBarTrack->setSize(Vector2(scrollBarTrack->getWidth(),
			barHeight - scrollBarUpButton->getHeight() * 2));

		maxPercent = percentForOneItem * (double) (totalItems - maxDisplayItems);

		/*wostringstream ws;
		ws << "percentShowing: " << percentShowing << "\n";
		ws << "(double) (totalItems - maxDisplayItems): " << (double) (totalItems - maxDisplayItems) << "\n";
		ws << "maxPercent: " << maxPercent << "\n";
		OutputDebugString(ws.str().c_str());*/

		Vector2 newButtonPos = scrollBarDownButton->getPosition();
		newButtonPos.y =
			scrollBarTrack->getPosition().y + scrollBarTrack->getHeight();
		scrollBarDownButton->setPosition(newButtonPos);
	}

	scrubber->setDimensions(scrollBarTrack.get(), percentShowing, maxPercent);

}


void ScrollBar::update(double deltaTime) {

	isHover = scrollBarTrack->getHitArea()->contains(mouse->getPosition());

	scrubber->update(deltaTime, mouse.get());

	if (!scrubber->hovering() && isHover && mouse->leftButton()) {

		if (firstClickTimer == 0) {
			bool up = mouse->getPosition().y > scrubber->getPosition().y;
			int dir = (up ? 5 : -5);
			scrubber->scroll(percentForOneItem * dir, scrubberPercentForOneItem * dir);
		}

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			bool up = mouse->getPosition().y > scrubber->getPosition().y;
			int dir = (up ? 5 : -5);
			scrubber->scroll(percentForOneItem * dir, scrubberPercentForOneItem * dir);
			firstClickTimer = autoScrollDelay;
		}

	}

	scrollBarDownButton->update(deltaTime);
	scrollBarUpButton->update(deltaTime);
	// change this to actionlisteners, or nah?
	if (scrollBarDownButton->pressed()) {
		// scroll down
		if (firstClickTimer == 0) {
			scrubber->scroll(percentForOneItem, scrubberPercentForOneItem);
		}

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			scrubber->scroll(percentForOneItem, scrubberPercentForOneItem);
			firstClickTimer = autoScrollDelay;
		}

	} else if (scrollBarUpButton->pressed()) {
		// scroll up
		if (firstClickTimer == 0)
			scrubber->scroll(-percentForOneItem, -scrubberPercentForOneItem);

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			scrubber->scroll(-percentForOneItem, -scrubberPercentForOneItem);
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


void ScrollBar::draw(SpriteBatch* batch) {

	// draw track bar
	scrollBarTrack->draw(batch);

	scrollBarDownButton->draw(batch);
	scrollBarUpButton->draw(batch);

	// draw scrubber
	scrubber->draw(batch);

}

void ScrollBar::setScrollPositionByPercent(double newPositionPercentage) {
	percentScroll = newPositionPercentage;
	scrubber->setScrollPositionByPercent(percentScroll);
}

void ScrollBar::scrollByIncrement(int scrollIncrement) {
	scrubber->scroll(scrollIncrement * percentForOneItem, scrollIncrement* scrubberPercentForOneItem);
}

const Vector2& ScrollBar::getPosition() const {
	return position;
}

const Vector2& ScrollBar::getSize() const {
	return Vector2(scrollBarDownButton->getWidth(), barHeight);
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

bool ScrollBar::pressed() {
	return isPressed;
}

bool ScrollBar::hovering() {
	return isHover;
}

void ScrollBar::setScale(const Vector2& newScale) {

	scrollBarTrack->setScale(newScale);
	scrubber->setScale(newScale);
	scrollBarUpButton->setScale(newScale);
	scrollBarDownButton->setScale(newScale);
}

/* Unused in ScrollBar. */
void ScrollBar::setFont(const pugi::char_t* font) {
}
/* Unused in ScrollBar. */
void ScrollBar::setText(wstring text) {
}
/* Unused in ScrollBar. */
const Vector2& XM_CALLCONV ScrollBar::measureString() const {
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


void Scrubber::setDimensions(const Sprite* scrollBarTrack,
	double percentShowing, double max) {

	minPosition = maxPosition = position = scrollBarTrack->getPosition();
	maxPercent = max;
	percentDifference = 1 / maxPercent;

	Vector2 size = Vector2(
		scrollBarTrack->getWidth(), scrollBarTrack->getHeight() * percentShowing);
	scrollBarHeight = scrollBarTrack->getHeight();

	if (!assetIsPixel) {
		width = size.x;
		height = size.y;
	}

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	maxPosition.y += scrollBarHeight - height;

	hitArea.reset(new HitArea(position, Vector2(width*scale.x, height*scale.y)));

	minMaxDifference = maxPosition.y - minPosition.y;
}

void Scrubber::update(double deltaTime, MouseController* mouse) {

	isHover = hitArea->contains(mouse->getPosition());

	if (isHover && mouse->leftButton() && !mouse->leftButtonLast()) {
		isPressed = true;
		pressedPosition = mouse->getPosition().y - position.y;

	} else if (!mouse->leftButton())
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

void Scrubber::setPosition(const Vector2& pos) {

	RectangleSprite::setPosition(pos);

}

void Scrubber::moveBy(const Vector2& moveVector) {

	RectangleSprite::moveBy(moveVector);
	minPosition += moveVector;
	maxPosition += moveVector;
}


void Scrubber::setScrollPositionByCoord(int newCoordinatePosition) {

	position.y = newCoordinatePosition;

	if (position.y < minPosition.y)
		position.y = minPosition.y;
	else if (position.y > maxPosition.y)
		position.y = maxPosition.y;

	hitArea->position.y = position.y;

	double distanceBetweenPosAndMax = maxPosition.y - position.y;

	scrubberPercentAt = (minMaxDifference - distanceBetweenPosAndMax)
		/ (minMaxDifference);
	percentAt = scrubberPercentAt/percentDifference;

}

void Scrubber::setScrollPositionByPercent(double newPositionPercentage) {

	percentAt = newPositionPercentage;
	scrubberPercentAt = percentAt*percentDifference;
	position.y = (minMaxDifference * scrubberPercentAt) + minPosition.y;
	hitArea->position = Vector2(position.x, position.y);
	hitArea->size = Vector2(width*scale.x, height*scale.y);
}


void Scrubber::scroll(double itemIncrement, double scrubberIncrement) {

	percentAt += itemIncrement;
	scrubberPercentAt += scrubberIncrement;
	if (scrubberPercentAt < abs(scrubberIncrement)) {
		percentAt = 0;
		scrubberPercentAt = 0;
	} else if (scrubberPercentAt > 1 - abs(scrubberIncrement)) {
		percentAt = maxPercent;
		scrubberPercentAt = 1;
	}
	position.y = (minMaxDifference * (scrubberPercentAt)) + minPosition.y;
	hitArea->position.y = position.y;
}


bool Scrubber::hovering() {
	return isHover;
}

bool Scrubber::pressed() {
	return isPressed;
}
/** **** Scrubber END **** **/