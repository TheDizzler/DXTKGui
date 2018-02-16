#include "ScrollBar.h"
#include "../GUIFactory.h"

/** **** SCROLLBAR **** **/
ScrollBar::ScrollBar(GUIFactory* factory, MouseController* mouseController,
	const Vector2& pos) : GUIControl(factory, mouseController) {

	position = pos;

}

ScrollBar::~ScrollBar() {
	scrollBarDownButton.reset();
	scrollBarUpButton.reset();
	scrubber.reset();
	scrollBarTrack.reset();
}


bool ScrollBar::initialize(GraphicsAsset* const pixelAsset,
	size_t barHght, ImageButton* scrollButtons[2], unique_ptr<Sprite> scrllBrTrck,
	GraphicsAsset* scrbbr) {

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
	Vector2 trackSize = Vector2((float) scrollBarUpButton->getWidth(),
		float(barHeight - scrollBarUpButton->getHeight() * 2));

	if (scrllBrTrck.get() == NULL) {
		scrollBarTrack.reset(guiFactory->createRectangle(scrollBarPosition, trackSize));
		scrollBarTrack->setTint(Color(.502f, .502f, .502f, 1));
	} else {
		scrollBarTrack = move(scrllBrTrck);
		scrollBarTrack->setOrigin(Vector2(0, 0));
		scrollBarTrack->setDimensions(scrollBarPosition, trackSize);
	}

	if (scrbbr == NULL) {
		scrubber = make_unique<Scrubber>(pixelAsset, false);

	} else {
		scrubber = make_unique<Scrubber>(scrbbr, true);
	}
	scrubber->setDimensions(scrollBarTrack.get(), 1, 0);
	return true;
}

void ScrollBar::reloadGraphicsAsset() {
	scrollBarUpButton->reloadGraphicsAsset();
	scrollBarDownButton->reloadGraphicsAsset();
	scrollBarTrack->reloadGraphicsAsset(guiFactory);
	scrollBarTrack->setOrigin(Vector2(0, 0));
	scrubber->reloadGraphicsAsset(guiFactory);
	scrubber->setOrigin(Vector2(0, 0));
}

void ScrollBar::moveBy(const Vector2& moveVector) {

	position += moveVector;
	scrollBarUpButton->moveBy(moveVector);
	scrollBarDownButton->moveBy(moveVector);
	scrollBarTrack->moveBy(moveVector);
	scrubber->moveBy(moveVector);
}

/* newPosition.x is position of right side of scrollbar's container. */
void ScrollBar::setPosition(const Vector2& newPosition) {
	/** Do not use ScrollBar's HitArea - it is NULL. */
	Vector2 moveVector = newPosition - position;
	position = newPosition;

	int xPos = (int) newPosition.x - scrollBarUpButton->getWidth();
	scrollBarUpButton->setPosition(Vector2((float) xPos, newPosition.y));

	scrollBarDownButton->setPosition(
		Vector2((float) xPos, newPosition.y + barHeight - scrollBarUpButton->getHeight()));

	scrollBarTrack->setPosition(
		Vector2((float) xPos, newPosition.y + scrollBarUpButton->getHeight()));

	/*double perc = scrubber->percentAt;
	Vector2 scrubberPos = scrollBarTrack->getPosition();
	scrubber->setPosition(scrubberPos);
	scrubber->setScrollPositionByPercent(perc);*/

	scrubber->moveBy(moveVector);
}

void ScrollBar::setBarHeight(size_t barHght) {
	barHeight = barHght;

	scrollBarDownButton->setPosition(
		Vector2(position.x - scrollBarDownButton->getWidth(),
			position.y + barHeight - scrollBarDownButton->getHeight()));


	// use this for HitArea
	Vector2 scrollBarPosition =
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y + scrollBarUpButton->getHeight());
	Vector2 trackSize = Vector2((float) scrollBarUpButton->getWidth(),
		float(barHeight - scrollBarUpButton->getHeight() * 2));

	/*Vector2 scrubberStartPos(
	scrollBarPosition.x,
	scrollBarPosition.y);*/

	scrubber->setDimensions(scrollBarTrack.get(), 1, -1);
}


void ScrollBar::setScrollBar(size_t totalItems, size_t itemHeight, size_t maxDisplayItems) {

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
		scrollBarTrack->setSize(Vector2((float) scrollBarTrack->getWidth(),
			float((totalItems * itemHeight) - scrollBarUpButton->getHeight() * 2)));

	} else {
		percentShowing = percentForOneItem * (maxDisplayItems);
		scrollBarTrack->setSize(Vector2((float) scrollBarTrack->getWidth(),
			float(barHeight - scrollBarUpButton->getHeight() * 2)));

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


bool ScrollBar::update(double deltaTime) {

	bool refreshed = false;
	isHover = scrollBarTrack->contains(mouse->getPosition());

	refreshed = scrubber->update(deltaTime, mouse);

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

	if (scrollBarDownButton->update(deltaTime))
		refreshed = true;
	if (scrollBarUpButton->update(deltaTime))
		refreshed = true;

	// change this to actionlisteners, or nah?
	if (scrollBarDownButton->pressed()) {
		// scroll down
		if (firstClickTimer == 0) {
			scrubber->scroll(percentForOneItem, scrubberPercentForOneItem);
			refreshed = true;
		}

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			scrubber->scroll(percentForOneItem, scrubberPercentForOneItem);
			firstClickTimer = autoScrollDelay;
			refreshed = true;
		}

	} else if (scrollBarUpButton->pressed()) {
		// scroll up
		if (firstClickTimer == 0) {
			scrubber->scroll(-percentForOneItem, -scrubberPercentForOneItem);
			refreshed = true;
		}

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			scrubber->scroll(-percentForOneItem, -scrubberPercentForOneItem);
			firstClickTimer = autoScrollDelay;
			refreshed = true;
		}

	}

	if (scrollBarDownButton->clicked()
		|| scrollBarUpButton->clicked()
		|| !(scrollBarDownButton->hovering()
			|| scrollBarUpButton->hovering()
			|| isHover))
		firstClickTimer = 0;

	percentScroll = scrubber->percentAt;

	/*if (refreshTexture) {
	texturePanel->set
	refreshTexture = false;
	return true;
	}*/
	return refreshed;
}


void ScrollBar::draw(SpriteBatch* batch) {

	//texturePanel->draw(batch);
	// draw track bar
	scrollBarTrack->draw(batch);

	scrollBarDownButton->draw(batch);
	scrollBarUpButton->draw(batch);

	// draw scrubber
	scrubber->draw(batch);

}

double ScrollBar::getPercentScroll() {
	return percentScroll;
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

const Vector2 ScrollBar::getSize() const {
	return Vector2((float) scrollBarDownButton->getWidth(), (float) barHeight);
}

const int ScrollBar::getWidth() const {
	return scrollBarDownButton->getWidth();
}

const int ScrollBar::getHeight() const {
	return (int) barHeight;
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

void ScrollBar::setLayerDepth(const float depth, bool frontToBack) {

	float nudge = .00000001f;
	if (!frontToBack)
		nudge *= -1;
	scrollBarTrack->setLayerDepth(depth, frontToBack);

	scrollBarDownButton->setLayerDepth(depth + nudge, frontToBack);
	scrollBarUpButton->setLayerDepth(depth + nudge * 2, frontToBack);

	// draw scrubber
	scrubber->setLayerDepth(depth + nudge * 3, frontToBack);
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
const Vector2 XM_CALLCONV ScrollBar::measureString() const {
	return Vector2::Zero;
}
/** **** ScrollBar END **** **/




/** **** Scrubber **** **/
Scrubber::Scrubber(GraphicsAsset* const graphicsAsset, bool pixel)
	: RectangleSprite(graphicsAsset) {

	isPixel = pixel;
	position = Vector2::Zero;
}

Scrubber::~Scrubber() {
}


void Scrubber::setDimensions(const Sprite* scrollBarTrack,
	double percentShowing, double max) {

	// save current percentage
	double currentpercent = percentAt;

	position = scrollBarTrack->getPosition();
	minPosition = maxPosition = (int) position.y;

	if (max != -1)
		maxPercent = max;
	percentDifference = 1 / maxPercent;

	Vector2 size = Vector2(
		(float) scrollBarTrack->getWidth(), float(scrollBarTrack->getHeight() * percentShowing));
	scrollBarHeight = scrollBarTrack->getHeight();

	if (!isPixel) {
		width = (UINT) size.x;
		height = (UINT) size.y;
	}

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.bottom = height;
	sourceRect.right = width;

	//maxPosition.y += scrollBarHeight - height;
	maxPosition += scrollBarHeight - height;
	//minMaxDifference = maxPosition.y - minPosition.y;
	minMaxDifference = maxPosition - minPosition;
	setScrollPositionByPercent(currentpercent);

	hitArea.position = position;
	hitArea.size = Vector2(width*scale.x, height*scale.y);

}

bool Scrubber::update(double deltaTime, MouseController* mouse) {

	bool refreshed = false;
	bool wasPressed = false;
	bool wasHover = isHover;
	isHover = hitArea.contains(mouse->getPosition());

	wasPressed = isPressed;

	if (isHover && mouse->leftButton() && !mouse->leftButtonLast()) {
		isPressed = true;
		pressedPosition = int(mouse->getPosition().y - position.y);
	} else if (!mouse->leftButton()) {
		isPressed = false;
	}

	if (isPressed) {
		tint = selectedColor;
		float newCoord = mouse->getPosition().y - pressedPosition;
		if (newCoord != position.y) {
			setScrollPositionByCoord(newCoord);
			refreshed = true;
		} else if (!wasPressed)
			refreshed = true;
		else
			refreshed = false;
	} else if (isHover && !wasHover) {
		tint = hoverColor;
		refreshed = true;
	} else if (!isHover && wasHover) {
		tint = normalColor;
		refreshed = true;
	} else if (wasPressed) {
		if (isHover)
			tint = hoverColor;
		else
			tint = normalColor;
		refreshed = true;
	}
	return refreshed;
}

void Scrubber::setPosition(const Vector2& pos) {
	RectangleSprite::setPosition(pos);
	/*Vector2 moveVector = position - pos;
	moveBy(moveVector);*/

}

void Scrubber::moveBy(const Vector2& moveVector) {

	//RectangleSprite::moveBy(moveVector);
	/*position += moveVector;
	hitArea->position = position;
	minPosition = position.y;
	maxPosition = position.y + minMaxDifference;*/
	RectangleSprite::moveBy(moveVector);
	minPosition += (int) moveVector.y;
	maxPosition += (int) moveVector.y;
}


void Scrubber::setScrollPositionByCoord(float newCoordinatePosition) {

	position.y = newCoordinatePosition;

	if (position.y < minPosition)
		position.y = (float) minPosition;
	else if (position.y > maxPosition)
		position.y = (float) maxPosition;

	hitArea.position.y = position.y;

	double distanceBetweenPosAndMax = maxPosition - position.y;

	scrubberPercentAt = (minMaxDifference - distanceBetweenPosAndMax)
		/ (minMaxDifference);
	percentAt = scrubberPercentAt / percentDifference;

}

void Scrubber::setScrollPositionByPercent(double newPositionPercentage) {

	percentAt = newPositionPercentage;
	scrubberPercentAt = percentAt * percentDifference;
	position.y = float((minMaxDifference * scrubberPercentAt) + minPosition);
	hitArea.position = Vector2(position.x, position.y);
	hitArea.size = Vector2(width*scale.x, height*scale.y);
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
	position.y = float((minMaxDifference * scrubberPercentAt) + minPosition);
	hitArea.position.y = position.y;
}


bool Scrubber::hovering() {
	return isHover;
}

bool Scrubber::pressed() {
	return isPressed;
}
/** **** Scrubber END **** **/