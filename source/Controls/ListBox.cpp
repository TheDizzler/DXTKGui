#include "ListBox.h"


ListBox::ListBox(const Vector2& pos,
	const int len, const int maxItemsShown) {

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
	scrollBar->setFactory(guiFactory);
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

	if (itemsToDisplay == maxDisplayItems || alwaysShowScrollBar) {
		scrollBar->scrollByIncrement(-mouse->getWheelDelta());
		scrollBar->update(deltaTime, mouse);
		firstItemToDisplay = (scrollBar->percentScroll)
			* (listItems.size() - maxDisplayItems);
	}

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

	// Adjust starting position of list to place the selected item into view.
	// Should only be relevant when the list is setup with an item selected.
	if (abs((float) firstItemToDisplay - selectedIndex) > maxDisplayItems) {

		if (listItems.size() - selectedIndex < maxDisplayItems)
			selectedIndex = listItems.size() - maxDisplayItems;

	}
	scrollBar->setScrollPositionByPercent(
		selectedIndex / (float) (listItems.size() - maxDisplayItems));

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

#include "../Controls/GUIFactory.h"
void ListBox::setFont(const pugi::char_t* fontName) {

	font = guiFactory->getFont(fontName);
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

