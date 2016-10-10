#include "ListBox.h"


ListBox::ListBox(const Vector2& pos, const int len,
	size_t itmHght, const int maxItemsShown) {

	position = pos;
	width = len;
	maxDisplayItems = maxItemsShown;
	action = SELECTION_CHANGED;
	itemHeight = itmHght;
}

ListBox::~ListBox() {

	if (onClickListener != NULL)
		delete onClickListener;

	for (ListItem* listItem : listItems)
		delete listItem;
	listItems.clear();

	delete emptyListItem;
}


void ListBox::initialize(shared_ptr<FontSet> fnt, GraphicsAsset* pixelAsset,
	ScrollBar* scrllbr, bool enumerateList) {

	font = fnt;
	pixel = pixelAsset->getTexture();

	firstItemPos = Vector2(position.x, position.y);

	scrollBar.reset(scrllbr);

	frame.reset(new RectangleFrame(pixelAsset));

	hitArea.reset(new HitArea(position, Vector2(frame->getWidth(), frame->getHeight())));

	isEnumerated = enumerateList;

	emptyListItem = new EmptyListItem();
	emptyListItem->initialize(width - scrollBar->getWidth(), itemHeight,
		font, pixel, listItems.size(), isEnumerated);
	emptyListItem->setText();
}

#include "GUIFactory.h"
void ListBox::setScrollBar(ScrollBarDesc& scrollBarDesc) {

	scrollBar.reset(guiFactory->createScrollBar(
		Vector2(position.x + width, position.y),
		itemHeight * maxDisplayItems, scrollBarDesc));
	scrollBar->setScrollBar(listItems.size(), itemHeight, maxDisplayItems);
}


void ListBox::addItem(ListItem* item) {

	listItems.push_back(item);
	if (item->measureString().x > +scrollBar->getWidth() > longestLabelLength)
		longestLabelLength = item->measureString().x;

	resizeBox();
}

/** Adds a vector of ListItems to listbox and clears the input vector.*/
void ListBox::addItems(vector<ListItem* > items) {

	for (ListItem* item : items) {
		item->initialize(width - scrollBar->getWidth(), itemHeight,
			font, pixel, listItems.size(), isEnumerated);
		listItems.push_back(item);
		if (item->measureString().x + scrollBar->getWidth() > longestLabelLength)
			longestLabelLength = item->measureString().x;
	}
	items.clear();

	resizeBox();

}


void ListBox::resizeBox() {

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
	if (listItems.size() > maxDisplayItems || alwaysDisplayScrollBar)
		frameWidth = width;
	else
		frameWidth = width - scrollBar->getWidth();

	int frameHeight = itemHeight * itemsToDisplay;
	Vector2 frameSize = Vector2(frameWidth, frameHeight);
	frame->setDimensions(position, frameSize, frameThickness);
	hitArea->size = frameSize;
}

void ListBox::alwaysShowScrollBar(bool alwaysShow) {
	alwaysDisplayScrollBar = alwaysShow;
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
	resizeBox();
}


void ListBox::update(double deltaTime) {

	if (itemsToDisplay == maxDisplayItems || alwaysDisplayScrollBar) {

		if (hitArea->contains(mouse->getPosition())) {
			int mouseWheelDelta = mouse->scrollWheelValue();
			if (mouseWheelDelta != 0)
				scrollBar->scrollByIncrement(-mouseWheelDelta);
		}

		scrollBar->update(deltaTime);

		double dif = listItems.size() /*- maxDisplayItems*/;
		firstItemToDisplay = round(scrollBar->percentScroll * (double) dif);
		/*if (firstItemToDisplay > listItems.size() - maxDisplayItems)
			firstItemToDisplay = listItems.size() - maxDisplayItems;*/
	}

	for (int j = firstItemToDisplay;
		j < firstItemToDisplay + itemsToDisplay; ++j) {
		if (listItems[j]->update(deltaTime, mouse.get())) {
			if (!multiSelect) {
				for (int i = 0; i < listItems.size(); ++i) {
					if (listItems[i] == listItems[j]) {
						selectedIndex = i;
						continue;
					}
					listItems[i]->isPressed = false;
				}
			}

			isClicked = true;
			onClick();
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

	if (itemsToDisplay == maxDisplayItems || alwaysDisplayScrollBar)
		scrollBar->draw(batch);

	if (listItems.size() > 0) { // draw frame
		frame->draw(batch);
	}

}


void ListBox::setSelected(size_t newIndex) {

	if (listItems.size() <= 0)
		return;

	selectedIndex = newIndex;
	if (!multiSelect) {
		for (ListItem* unselect : listItems) {
			unselect->isPressed = false;
		}
	}
	listItems[selectedIndex]->isPressed = true;

	// Adjust starting position of list to place the pressed item into view.
	// Should only be relevant when the list is setup with an item pressed.
	if (abs((float) firstItemToDisplay - selectedIndex) > maxDisplayItems) {

		if (listItems.size() - selectedIndex < maxDisplayItems)
			selectedIndex = listItems.size() - maxDisplayItems;

	}
	scrollBar->setScrollPositionByPercent(
		selectedIndex / (double) (listItems.size() /*- maxDisplayItems*/));

}

const int ListBox::getSelectedIndex() const {
	return selectedIndex;
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
const Vector2& XM_CALLCONV ListBox::measureString() const {
	return XMVECTOR();
}


void ListBox::setFont(const pugi::char_t* fontName) {

	font = guiFactory->getFont(fontName);
}


const Vector2& ListBox::getPosition() const {
	return position;
}

const int ListBox::getWidth() const {
	return frame->getWidth();
}

const int ListBox::getHeight() const {

	int numItems = listItems.size() > maxDisplayItems ? maxDisplayItems : listItems.size();
	int height = numItems * itemHeight;
	return height;
}


bool ListBox::clicked() {

	if (isClicked) {
		//action = ClickAction::NONE;
		isClicked = isPressed = false;
		return true;
	}

	return false;
}
bool ListBox::pressed() {
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

	if ((isHover = hitArea->contains(mouse->getPosition()))) {

		if (mouse->leftButton() && !buttonDownLast)
			buttonDownLast = true;

		else if (!mouse->leftButton() && buttonDownLast) {
			isPressed = true;
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

	if (isPressed) {// draw pressed color bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			::DirectX::Colors::White, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, 0.0f);
		textLabel->draw(batch, Color(0, 0, 0, 1));

	} else if (isHover) { // draw hover color bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			::DirectX::Colors::Aqua, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, 0.0f);
		textLabel->draw(batch);

	} else { // draw basic bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			::DirectX::Colors::BurlyWood, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, 0.0f);
		textLabel->draw(batch);
	}
}
/** **** ListItem END **** **/

