#include "ListBox.h"
#include "../GUIFactory.h"

ListBox::ListBox(GUIFactory* factory, MouseController* mouseController,
	const Vector2& pos, const int len, size_t itmHght, const int maxItemsShown)
	: GUIControl(factory, mouseController) {

	position = pos;
	width = len;
	maxDisplayItems = maxItemsShown;
	itemHeight = itmHght;
}


ListBox::~ListBox() {

	if (scrollBar != NULL || scrollBar.get())
		scrollBar.reset();

	if (actionListener != NULL)
		delete actionListener;

	for (ListItem* listItem : listItems)
		delete listItem;
	listItems.clear();

	delete emptyListItem;
}


void ListBox::initialize(const pugi::char_t* fnt, GraphicsAsset* pixelAsset,
	ScrollBar* scrllbr, bool enumerateList, const int frmThck) {

	fontName = fnt;
	pixel = pixelAsset->getTexture();

	frameThickness = frmThck;
	firstItemPos = Vector2(position.x, position.y);
	scrollBar.reset(scrllbr);
	frame.reset(guiFactory->createRectangleFrame());
	hitArea.position = position;
	hitArea.size = Vector2((float) frame->getWidth(), (float) frame->getHeight());
	isEnumerated = enumerateList;

	texturePanel.reset(guiFactory->createPanel(true));
	texturePanel->setTexturePosition(firstItemPos);

	emptyListItem = new EmptyListItem();
	emptyListItem->initialize(width - scrollBar->getWidth(), (int) itemHeight,
		guiFactory->createTextLabel(Vector2::Zero, L"", fontName),
		pixel, listItems.size(), isEnumerated);
	emptyListItem->setText();

	refreshTexture = true;
}

void ListBox::forceRefresh() {
	refreshTexture = true;
}

void ListBox::reloadGraphicsAsset() {
	pixel = guiFactory->getAsset("White Pixel")->getTexture();
	scrollBar->reloadGraphicsAsset();
	//frame.reset(guiFactory->createRectangleFrame());
	frame->reloadGraphicsAsset();
	texturePanel.reset(guiFactory->createPanel(true));
	texturePanel->setTexturePosition(firstItemPos);
	for (size_t i = firstItemToDisplay; i < firstItemToDisplay + itemsToDisplay; ++i)
		listItems[i]->reloadGraphicsAsset(guiFactory);

	resizeBox();

}


bool ListBox::update(double deltaTime) {

	if (itemsToDisplay == maxDisplayItems || alwaysDisplayScrollBar) {

		if (hitArea.contains(mouse->getPosition())) {
			int mouseWheelDelta = mouse->scrollWheelValue();
			if (mouseWheelDelta != 0) {
				scrollBar->scrollByIncrement(-mouseWheelDelta);
				refreshTexture = true;
			}
		}

		if (scrollBar->update(deltaTime))
			refreshTexture = true;

		double dif = (double) listItems.size();
		firstItemToDisplay = (int) round(scrollBar->getPercentScroll() * (double) dif);

	}

	for (size_t j = firstItemToDisplay; j < firstItemToDisplay + itemsToDisplay; ++j) {
		if (listItems[j]->update(deltaTime, mouse)) {
			refreshTexture = true;
			if (listItems[j]->isSelected) {
				if (!multiSelect) {
					for (int i = 0; i < listItems.size(); ++i) {
						if (listItems[i] == listItems[j]) {
							if (selectedIndex != i) {
								selectedIndex = i;
								onClick();
							}
							continue;
						}
						if (listItems[i]->isSelected)
							listItems[i]->setSelected(false);
					}
				}
			} else if (listItems[j]->isHovered) {
				hoveredIndex = (int) j;
				onHover();
			} else {
				hoveredIndex = -1;
			}
		}
	}

	if (refreshTexture) {
		frame->update();

		texturePanel->setTexture(texturize());
		refreshTexture = false;
		return true;
	}
	return false;
}


void ListBox::draw(SpriteBatch* batch) {
	texturePanel->draw(batch);
}


void ListBox::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {

	for (size_t i = firstItemToDisplay; i < firstItemToDisplay + itemsToDisplay; ++i) {
		listItems[i]->draw(batch);
	}

	if (itemsToDisplay == maxDisplayItems || alwaysDisplayScrollBar) {
		scrollBar->draw(batch);
	}

	if (listItems.size() > 0) {
		frame->draw(batch);
	}
}

unique_ptr<GraphicsAsset> ListBox::texturize() {
	return guiFactory->createTextureFromTexturizable(this);
}

void ListBox::setScrollBar(ScrollBarDesc& scrollBarDesc) {

	scrollBar.reset(guiFactory->createScrollBar(
		Vector2(position.x + width, position.y),
		itemHeight * maxDisplayItems, scrollBarDesc));
	scrollBar->setScrollBar(listItems.size(), itemHeight, maxDisplayItems);
}

void ListBox::addItem(ListItem* item) {

	listItems.push_back(item);
	if (item->measureString().x + scrollBar->getWidth() > longestLabelLength)
		longestLabelLength = (int) item->measureString().x;

	resizeBox();
}

/** Adds a vector of ListItems to listbox and clears the input vector.*/
void ListBox::addItems(vector<ListItem*> items) {

	for (ListItem* item : items) {
		item->initialize(width - scrollBar->getWidth(), (int) itemHeight,
			guiFactory->createTextLabel(Vector2::Zero, L"", fontName),
			pixel, listItems.size(), layerDepth, isEnumerated);
		listItems.push_back(item);
		if (item->measureString().x + scrollBar->getWidth() > longestLabelLength)
			longestLabelLength = (int) item->measureString().x;
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
	float frameWidth;
	if (listItems.size() > maxDisplayItems || alwaysDisplayScrollBar)
		frameWidth = (float) width;
	else
		frameWidth = (float) width - scrollBar->getWidth();

	float frameHeight = (float) itemHeight * itemsToDisplay;
	Vector2 frameSize = Vector2(frameWidth, frameHeight);
	frame->setDimensions(position, frameSize, frameThickness);
	hitArea.size = frameSize;

	refreshTexture = true;
}


void ListBox::moveBy(const Vector2& moveVector) {
	position += moveVector;
	hitArea.position = Vector2(position.x, position.y);
	hitArea.size = Vector2(getWidth()*scale.x, getHeight()*scale.y);
	firstItemPos += moveVector;

	Vector2 pos = firstItemPos;

	for (size_t i = firstItemToDisplay; i < firstItemToDisplay + itemsToDisplay; ++i) {
		listItems[i]->updatePosition(pos);
		pos.y += itemHeight;
	}
	scrollBar->moveBy(moveVector);
	frame->moveBy(moveVector);

	texturePanel->moveBy(moveVector);
}

void ListBox::setPosition(const Vector2& newPosition) {
	Vector2 moveVector = newPosition - position;
	position = newPosition;
	hitArea.position = Vector2(position.x, position.y);
	hitArea.size = Vector2(getWidth()*scale.x, getHeight()*scale.y);

	firstItemPos += moveVector;
	Vector2 pos = firstItemPos;

	for (size_t i = firstItemToDisplay; i < firstItemToDisplay + itemsToDisplay; ++i) {
		listItems[i]->updatePosition(pos);
		pos.y += itemHeight;
	}
	//scrollBar->setPosition(Vector2(position.x + width, position.y));
	scrollBar->moveBy(moveVector);
	frame->setPosition(position);

	texturePanel->moveBy(moveVector);
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
	selectedIndex = 0;
	for (ListItem* listItem : listItems)
		delete listItem;
	listItems.clear();
	resizeBox();
	scrollBar->setScrollPositionByPercent(0);
}

void ListBox::setSelected(size_t newIndex) {

	if (listItems.size() <= 0)
		return;

	selectedIndex = newIndex;
	if (!multiSelect) {
		for (ListItem* unselect : listItems) {
			if (unselect->isSelected)
				unselect->setSelected(false);
		}
	}

	listItems[selectedIndex]->setSelected(true);
	// Adjust starting position of list to place the pressed item into view.
	// Should only be relevant when the list is setup with an item pressed.
	if (abs((float) firstItemToDisplay - selectedIndex) > maxDisplayItems) {

		if (listItems.size() - selectedIndex < maxDisplayItems)
			selectedIndex = listItems.size() - maxDisplayItems;

	}
	scrollBar->setScrollPositionByPercent(
		selectedIndex / (double) (listItems.size()));

	refreshTexture = true;
}

const size_t ListBox::getSelectedIndex() const {
	return selectedIndex;
}

const int ListBox::getHoveredIndex() const {
	return hoveredIndex;
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
const Vector2 XM_CALLCONV ListBox::measureString() const {
	return Vector2::Zero;
}

void ListBox::setFont(const pugi::char_t* fnt) {
	fontName = fnt;
	refreshTexture = true;
}

const Vector2& ListBox::getPosition() const {
	return position;
}

const int ListBox::getWidth() const {
	return frame->getWidth();
}

const int ListBox::getHeight() const {

	size_t numItems = listItems.size() > maxDisplayItems ? maxDisplayItems : listItems.size();
	size_t height = numItems * itemHeight;
	return (int) height;
}

void ListBox::setLayerDepth(const float depth, bool frontToBack) {

	float nudge = .00000001f;
	if (!frontToBack)
		nudge *= -1;

	for (size_t i = firstItemToDisplay; i < firstItemToDisplay + itemsToDisplay; ++i)
		listItems[i]->setLayerDepth(depth);

	if (itemsToDisplay == maxDisplayItems || alwaysDisplayScrollBar)
		scrollBar->setLayerDepth(depth + nudge, frontToBack);

	if (listItems.size() > 0) // draw frame
		frame->setLayerDepth(depth + nudge * 2, frontToBack);
}


bool ListBox::clicked() {

	if (isClicked) {
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
ListItem::~ListItem() {

}

void ListItem::initialize(const int width, const int height,
	TextLabel* label, ComPtr<ID3D11ShaderResourceView> pixelTexture,
	size_t listPos, float layDep, bool enumerateList) {

	itemRect.left = 0;
	itemRect.top = 0;
	itemRect.bottom = height;
	itemRect.right = width;

	hitArea.reset(new HitArea(Vector2::Zero, Vector2((float) width, (float) height)));

	pixel = pixelTexture;
	itemPosition = Vector2::Zero;

	selectedBGColor = ::DirectX::Colors::White;
	hoverBGColor = ::DirectX::Colors::Aqua;
	normalBGColor = ::DirectX::Colors::BurlyWood;

	normalFontColor = label->getTint();
	textLabel.reset(label);

	isEnumerated = enumerateList;
	listPosition = listPos;

	layerDepth = layDep;

	setText();
}

void ListItem::reloadGraphicsAsset(GUIFactory* guiFactory) {
	pixel = guiFactory->getAsset("White Pixel")->getTexture();
	textLabel->reloadGraphicsAsset();
}

void ListItem::setWidth(int newWidth) {

	itemRect.right = newWidth;
	hitArea->size.x = (float) newWidth;
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

	bool refresh = false;
	bool wasHover = isHovered;
	if ((isHovered = hitArea->contains(mouse->getPosition()))) {

		if (mouse->leftButton() && !buttonDownLast) {
			buttonDownLast = true;
		} else if (!mouse->leftButton() && buttonDownLast) {
			isSelected = true;
			currentBGColor = selectedBGColor;
			textLabel->setTint(selectedFontColor);
			buttonDownLast = false;
			refresh = true;
		}
	} else {
		buttonDownLast = false;
	}


	if ((isHovered && !wasHover) || (!isHovered && wasHover))
		refresh = true;
	if (textLabel->update(deltaTime))
		refresh = true;
	return refresh;
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

	if (isSelected) {// draw pressed color bg
		if (isHovered)
			batch->Draw(pixel.Get(), itemPosition, &itemRect,
				selectedAndHoveredColor, 0.0f, Vector2(0, 0), Vector2(1, 1),
				SpriteEffects_None, layerDepth);
		else
			batch->Draw(pixel.Get(), itemPosition, &itemRect,
				selectedBGColor, 0.0f, Vector2(0, 0), Vector2(1, 1),
				SpriteEffects_None, layerDepth);
		textLabel->draw(batch);

	} else if (isHovered) { // draw hover color bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			hoverBGColor, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, layerDepth);
		textLabel->draw(batch);

	} else { // draw basic bg

		batch->Draw(pixel.Get(), itemPosition, &itemRect,
			normalBGColor, 0.0f, Vector2(0, 0), Vector2(1, 1),
			SpriteEffects_None, layerDepth);
		textLabel->draw(batch);
	}
}

void ListItem::setLayerDepth(const float depth) {
	layerDepth = depth;
}


void ListItem::setSelected(bool select) {

	isSelected = select;
	if (isSelected) {
		textLabel->setTint(selectedFontColor);
	} else
		textLabel->setTint(normalFontColor);
}
/** **** ListItem END **** **/

