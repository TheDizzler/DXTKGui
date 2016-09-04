#include "ListBox.h"


ListBox::ListBox(const Vector2& pos, const int len, const int maxItemsShown) {

	position = pos;
	width = len;
	maxDisplayItems = maxItemsShown;
	action = SELECTION_CHANGED;

}

ListBox::~ListBox() {

	if (onClickListener != NULL)
		delete onClickListener;

	for (ListItem* listItem : listItems)
		delete listItem;
	listItems.clear();

	delete emptyListItem;
}


void ListBox::initialize(shared_ptr<FontSet> fnt,
	ComPtr<ID3D11ShaderResourceView> whitePixel, bool enumerateList) {

	font = fnt;
	pixel = whitePixel;

	firstItemPos = Vector2(position.x, position.y);

	scrollBar.reset(new ScrollBar(Vector2(position.x + width, position.y)));
	if (!scrollBar->initialize(pixel, itemHeight * maxDisplayItems)) {
		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
	}

	frame.reset(new RectangleFrame(pixel));

	isEnumerated = enumerateList;

	emptyListItem = new EmptyListItem();
	emptyListItem->initialize(width - scrollBar->getWidth(), itemHeight,
		font, pixel, listItems.size(), isEnumerated);
	emptyListItem->setText();
}


/** Adds a vector of ListItems to listbox and clears the vector parameter.*/
void ListBox::addItems(vector<ListItem* > items) {

	for (ListItem* item : items) {
		item->initialize(width - scrollBar->getWidth(), itemHeight,
			font, pixel, listItems.size(), isEnumerated);
		listItems.push_back(item);
		if (item->measureString().x + scrollBar->getWidth() > longestLabelLength)
			longestLabelLength = item->measureString().x;
	}
	items.clear();

	if (longestLabelLength > width) {
		setWidth(longestLabelLength + scrollBar->getWidth());
		for (ListItem* item : listItems)
			item->setWidth(width - scrollBar->getWidth());

	}

	itemsToDisplay = maxDisplayItems;
	if (listItems.size() < itemsToDisplay)
		itemsToDisplay = listItems.size();

	scrollBar->setScrollBar(listItems.size(), itemHeight, maxDisplayItems);
	int frameWidth;
	if (listItems.size() > maxDisplayItems || alwaysShowScrollBar)
		frameWidth = width;
	else
		frameWidth = width - scrollBar->getWidth();

	int frameHeight = itemHeight * itemsToDisplay;
	frame->setDimensions(position, Vector2(frameWidth, frameHeight), frameThickness);
}


void ListBox::setWidth(int newWidth) {

	width = newWidth;
	scrollBar->setPosition(Vector2(position.x + width, position.y));
}

void ListBox::clear() {
	firstItemToDisplay = 0;
	for (ListItem* listItem : listItems)
		delete listItem;
	listItems.clear();
}


void ListBox::update(double deltaTime, MouseController* mouse) {

	for (ListItem* item : listItems) {
		if (item->update(deltaTime, mouse)) {
			if (!multiSelect) {
				for (int i = 0; i < listItems.size(); ++i) {
					if (listItems[i] == item) {
						selectedIndex = i;
						continue;
					}
					listItems[i]->isSelected = false;
				}
			}

			isClicked = true;
			triggerOnClick();
		}
	}


	if (itemsToDisplay == maxDisplayItems || alwaysShowScrollBar) {
		scrollBar->update(deltaTime, mouse);
		firstItemToDisplay = (scrollBar->percentScroll)
			* (listItems.size() - maxDisplayItems);

		/*wostringstream ws;
		ws << "\n" << "%: " << scrollBar->percentScroll;
		OutputDebugString(ws.str().c_str());*/
	}

	Vector2 pos = firstItemPos;

	for (int i = firstItemToDisplay;
		i < firstItemToDisplay + itemsToDisplay; ++i) {

		listItems[i]->updatePosition(pos);
		pos.y += itemHeight;
	}

}


void ListBox::draw(SpriteBatch* batch) {

	size_t shown = 0;
	for (int i = firstItemToDisplay;
		i < firstItemToDisplay + itemsToDisplay; ++i) {

		listItems[i]->draw(batch);

	}

	if (itemsToDisplay == maxDisplayItems || alwaysShowScrollBar)
		scrollBar->draw(batch);

	if (listItems.size() > 0) { // draw frame
		frame->draw(batch);
	}

}


#include <cmath>
void ListBox::setSelected(size_t newIndex) {

	if (listItems.size() <= 0)
		return;

	selectedIndex = newIndex;
	if (!multiSelect) {
		for (ListItem* unselect : listItems) {
			unselect->isSelected = false;
		}
	}
	listItems[selectedIndex]->isSelected = true;

	// Adjust starting position of list to place the selected in view
	// should only be relevant when the list is setup with an item selected.
	if (abs((float) firstItemToDisplay - selectedIndex) > maxDisplayItems) {

		if (listItems.size() - selectedIndex < maxDisplayItems)
			selectedIndex = listItems.size() - maxDisplayItems;

	}
	scrollBar->setScrollPositionByPercent(selectedIndex / (float) (listItems.size() - maxDisplayItems));

}


ListItem* ListBox::getSelected() {
	if (listItems.size() <= 0)
		return emptyListItem;
	return listItems[selectedIndex];
}

ListItem* ListBox::getItem(size_t index) {
	return listItems[index];
}

// do nothing for now
void ListBox::setText(wstring text) {
	// do nothing for now
}

// do nothing for now
XMVECTOR XM_CALLCONV ListBox::measureString() const {
	return XMVECTOR();
}


#include "../Managers/GameManager.h"
void ListBox::setFont(const pugi::char_t* fontName) {

	font = GameManager::guiFactory->getFont(fontName);
}


const Vector2& ListBox::getPosition() const {
	return position;
}

const int ListBox::getWidth() const {
	return width;
}

const int ListBox::getHeight() const {

	int numItems = listItems.size() > maxDisplayItems ? maxDisplayItems : listItems.size();
	int height = numItems * itemHeight;
	return height;
}


bool ListBox::clicked() {

	if (isClicked) {
		//action = ClickAction::NONE;
		isClicked = isSelected = false;
		return true;
	}

	return false;
}
bool ListBox::selected() {
	return false;
}
bool ListBox::hovering() {
	return false;
}

/** **** ListBox END **** **/





/** **** ListItem **** **/
ListItem::ListItem() {
}

ListItem::~ListItem() {
}

void ListItem::initialize(const int width, const int height,
	shared_ptr<FontSet> fnt, ComPtr<ID3D11ShaderResourceView> pixelTexture,
	size_t listPos, bool enumerateList) {

	itemRect.left = 0;
	itemRect.top = 0;
	itemRect.bottom = height;
	itemRect.right = width;

	hitArea.reset(new HitArea(
		Vector2::Zero, Vector2(width, height)));

	pixel = pixelTexture;
	itemPosition = Vector2::Zero;

	textLabel.reset(new TextLabel(fnt));

	isEnumerated = enumerateList;
	listPosition = listPos;
	setText();
}

void ListItem::setWidth(int newWidth) {

	itemRect.right = newWidth;
	hitArea->size.x = newWidth;
}

Vector2 ListItem::measureString() const {
	Vector2 size = textLabel->measureString();
	size.x += textMarginX * 2;
	size.y += textMarginY * 2;
	return size;
}

const wchar_t* ListItem::toString() {
	return textLabel->getText();
}


bool ListItem::update(double deltaTime, MouseController* mouse) {

	if ((isHover = hitArea->contains(mouse->getPosition())) == true) {

		if (mouse->leftButtonDown() && !buttonDownLast)
			buttonDownLast = true;

		else if (!mouse->leftButtonDown() && buttonDownLast) {
			isSelected = true;
			buttonDownLast = false;
			return true;
		}

	} else
		buttonDownLast = false;

	return false;
}

void ListItem::updatePosition(const Vector2& pos) {

	itemPosition = pos;
	hitArea->position = itemPosition;

	Vector2 position(pos);
	position.x += textMarginX;
	position.y += textMarginY;
	textLabel->setPosition(position);
}


void ListItem::draw(SpriteBatch* batch) {

	if (isSelected) {// draw selected color bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			::DirectX::Colors::White.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, 0.0f);
		textLabel->draw(batch, ::DirectX::Colors::Black.v);

	} else if (isHover) { // draw hover color bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			::DirectX::Colors::Aqua.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, 0.0f);
		textLabel->draw(batch);

	} else { // draw basic bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			::DirectX::Colors::BurlyWood.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, 0.0f);
		textLabel->draw(batch);
	}
}
/** **** ListItem END **** **/





/** **** SCROLLBAR **** **/
ScrollBar::ScrollBar(Vector2 pos) {

	position = pos;
}

ScrollBar::~ScrollBar() {
}

#include "../Managers/GameManager.h"
bool ScrollBar::initialize(ComPtr<ID3D11ShaderResourceView> pixelTexture,
	size_t maxHght) {

	maxHeight = maxHght;

	scrollBarDownButton.reset((ImageButton*)
		GameManager::guiFactory->createImageButton("ScrollBar Down",
			"ScrollBar Down Pressed"));
	scrollBarDownButton->setPosition(
		Vector2(position.x - scrollBarDownButton->getWidth(),
			position.y + maxHeight
			- scrollBarDownButton->getHeight()));

	scrollBarDownButton->action = Button::DOWN;


	scrollBarUpButton.reset((ImageButton*)
		GameManager::guiFactory->createImageButton("ScrollBar Up",
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
