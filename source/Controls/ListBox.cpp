#include "ListBox.h"


ListBox::ListBox(const Vector2& pos, const int len) {

	position = pos;
	width = len;

}

ListBox::~ListBox() {

	for (ListItem* listItem : listItems)
		delete listItem;

}


#include "DDSTextureLoader.h"
#include "../globals.h"
	//bool ListBox::initialize(ID3D11Device* device, const wchar_t* fontFile,
		//ID3D11ShaderResourceView* whitePixel) {

		//pixel = whitePixel;

		//font.reset(new FontSet());
		//if (!font->load(device, fontFile))
		//	return false;
		//font->setTint(DirectX::Colors::White.v);


		//if (Globals::reportError(DirectX::CreateDDSTextureFromFile(
		//	device, Assets::whitePixelFile, NULL, whiteBG.GetAddressOf()))) {

		//	MessageBox(NULL, L"Failed to create texture from WhitePixel.dds",
		//		L"ERROR", MB_OK);
		//	return false;
		//}

		//spaceBetweenItems = 32;
		/*firstItemPos = Vector2(position.x, position.y);


		scrollBar.reset(new ScrollBar(Vector2(position.x + width, position.y)));
		if (!scrollBar->initialize(device, pixel,
			itemHeight * maxDisplayItems)) {

			MessageBox(NULL, L"Failed to create ScrollBar",
				L"GUI initialization ERROR", MB_OK);
			return false;
		}

		return true;
	}*/

void ListBox::initialize(shared_ptr<FontSet> fnt,
	ComPtr<ID3D11ShaderResourceView> whitePixel) {

	font = fnt;
	pixel = whitePixel;

	firstItemPos = Vector2(position.x, position.y);

	scrollBar.reset(new ScrollBar(Vector2(position.x + width, position.y)));
	if (!scrollBar->initialize(pixel, itemHeight * maxDisplayItems)) {
		MessageBox(NULL, L"Failed to create ScrollBar",
			L"GUI initialization ERROR", MB_OK);
		//return false;
	}

	//return true;

}



void ListBox::addItems(vector<ListItem*> items) {

	for (ListItem* item : items) {
		item->initialize(width - scrollBar->getWidth(), itemHeight,
			font, pixel);
		listItems.push_back(item);
	}

	itemsToDisplay = maxDisplayItems;
	if (listItems.size() < itemsToDisplay)
		itemsToDisplay = listItems.size();

	scrollBar->setScrollBar(listItems.size(), itemHeight, maxDisplayItems);

}


bool ListBox::update(double deltaTime, MouseController* mouse) {

	bool changesMade = false;
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
			changesMade = true;
		}
	}


	if (itemsToDisplay == maxDisplayItems || alwaysShowScrollBar) {
		scrollBar->update(deltaTime, mouse);
		firstItemToDisplay = (scrollBar->percentScroll)
			* (listItems.size() - maxDisplayItems);

		/*wostringstream ws;
		ws << "\n" << "%: " << scrollBar->percentScroll;
		OutputDebugString(ws.str().c_str());*/

		/*if (firstItemToDisplay > listItems.size() - maxDisplayItems)
			firstItemToDisplay = listItems.size() - maxDisplayItems;*/
	}

	Vector2 pos = firstItemPos;

	for (int i = firstItemToDisplay;
		i < firstItemToDisplay + itemsToDisplay; ++i) {

		listItems[i]->updatePosition(pos);
		pos.y += itemHeight;
	}

	return changesMade;
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
		drawFrame(batch);
	}

}

/** THIS CAN BE OPTIMIZED. **/
void ListBox::drawFrame(SpriteBatch* batch) {

	int realWidth;
	if (listItems.size() > maxDisplayItems || alwaysShowScrollBar)
		realWidth = width;
	else
		realWidth = width - scrollBar->getWidth();
		// upper horizontal frame
	RECT frame;
	frame.left = 0;
	frame.top = 0;

	frame.right = realWidth;
	frame.bottom = frameThickness; // thickness of frame
	Vector2 framePos(position.x, position.y);

	batch->Draw(pixel.Get(), framePos, &frame,
		::DirectX::Colors::Black.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// lower horizontal frame
	int height = itemHeight * itemsToDisplay;
	framePos.y += height;

	batch->Draw(pixel.Get(), framePos, &frame,
		::DirectX::Colors::Black.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// left vertical frame
	framePos.y = position.y;
	frame.right = frameThickness;
	frame.bottom = height;

	batch->Draw(pixel.Get(), framePos, &frame,
		::DirectX::Colors::Black.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

	// right vertical frame
	framePos.x += realWidth;

	batch->Draw(pixel.Get(), framePos, &frame,
		::DirectX::Colors::Black.v, 0.0f, Vector2(0, 0), Vector2(1, 1),
		SpriteEffects_None, 0.0f);

}


void ListBox::drawSelected(SpriteBatch* batch, const Vector2& selectedPosition) {

	font->draw(batch, listItems[selectedIndex]->toString(), selectedPosition);
}


void ListBox::setSelected(size_t newIndex) {

	selectedIndex = newIndex;
	if (!multiSelect) {
		for (ListItem* unselect : listItems) {
			unselect->isSelected = false;
		}
	}
	listItems[selectedIndex]->isSelected = true;
}


ListItem * ListBox::getSelected() {
	return listItems[selectedIndex];
}
/** **** ListBox END **** **/





/** **** ListItem **** **/
ListItem::ListItem() {
}

ListItem::~ListItem() {
}

void ListItem::initialize(const int width, const int height,
	shared_ptr<FontSet> fnt, ComPtr<ID3D11ShaderResourceView> pixelTexture) {

	itemRect.left = 0;
	itemRect.top = 0;
	itemRect.bottom = height;
	itemRect.right = width;

	hitArea.reset(new HitArea(
		Vector2::Zero, Vector2(width, height)));

	pixel = pixelTexture;
	itemPosition = Vector2::Zero;

	textLabel.reset(new TextLabel(fnt));

	setText();
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
		}

	} else
		buttonDownLast = false;

	return isSelected;
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
		GameManager::guiManager->createImageButton("Arial", "ScrollBar Down Button",
			"ScrollBar Down Pressed"));
	scrollBarDownButton->setPosition(
		Vector2(position.x - scrollBarDownButton->getWidth() / 2,
			position.y + maxHeight
			- scrollBarDownButton->getHeight() / 2));

	scrollBarDownButton->action = Button::DOWN;


	scrollBarUpButton.reset((ImageButton*)
		GameManager::guiManager->createImageButton("Arial", "ScrollBar Up Button",
			"ScrollBar Up Pressed"));
	scrollBarUpButton->setPosition(
		Vector2(position.x - scrollBarUpButton->getWidth() / 2,
			position.y + scrollBarUpButton->getHeight() / 2));

	scrollBarUpButton->action = Button::UP;


	scrollBarPosition =
		Vector2(position.x - scrollBarUpButton->getWidth(),
			position.y + scrollBarUpButton->getHeight());
	scrollBarRect.left = 0;
	scrollBarRect.top = 0;
	scrollBarRect.bottom = maxHeight
		- scrollBarUpButton->getHeight() * 2;
	scrollBarRect.right = scrollBarUpButton->getWidth();

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



void ScrollBar::setScrollBar(int totalItems, int itemHeight, int maxDisplayItems) {

	//float totalListHeight = totalItems*itemHeight;
	percentForOneItem = (double) 1 / (totalItems - maxDisplayItems);
	//float percentToShow = maxHeight / (float) totalListHeight;
	double percentToShow;
	if (totalItems < maxDisplayItems) {
		percentToShow = 1;
		percentForOneItem = 0;
		scrollBarRect.bottom = (totalItems * itemHeight)
			- scrollBarUpButton->getHeight() * 2;

		Vector2 newButtonPos = scrollBarDownButton->getPosition();
		newButtonPos.y = scrollBarPosition.y + scrollBarRect.bottom + scrollBarDownButton->getHeight() / 2;
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

	// update scrubber
	scrubber->update(deltaTime, mouse);

	scrollBarDownButton->update(deltaTime, mouse);
	scrollBarUpButton->update(deltaTime, mouse);
	if (scrollBarDownButton->selected()) {
		// scroll down
		if (firstClickTimer == 0)
			scrubber->scroll(percentForOneItem);

		firstClickTimer += deltaTime;
		if (firstClickTimer >= autoScrollStartDelay) {
			// start autoscrolling
			//OutputDebugString(L"Go gogogog!");
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
			|| scrollBarUpButton->hovering()))
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



int ScrollBar::getWidth() {
	return scrollBarDownButton->getWidth();
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


//void Scrubber::setSize(const Vector2& size) {
//
//	width = size.x;
//	height = size.y;
//
//	sourceRect.left = 0;
//	sourceRect.top = 0;
//	sourceRect.bottom = height;
//	sourceRect.right = width;
//
//	maxPosition = minPosition;
//	maxPosition.y += scrollBarHeight - height;
//
//	hitArea.reset(new HitArea(minPosition,
//		Vector2(width*scale.x, height*scale.y)));
//
//	minMaxDifference = maxPosition.y - minPosition.y;
//}


//void Scrubber::setScrubberHeight(double newHeight) {
//
//	height = newHeight;
//
//	sourceRect.bottom = newHeight;
//
//	maxPosition.y += scrollBarHeight - newHeight;
//
//	hitArea.reset(new HitArea(minPosition,
//		Vector2(width*scale.x, newHeight*scale.y)));
//
//	minMaxDifference = maxPosition.y - minPosition.y;
//
//}

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
		position.y = mouse->getPosition().y - pressedPosition;

		if (position.y < minPosition.y)
			position.y = minPosition.y;
		else if (position.y > maxPosition.y)
			position.y = maxPosition.y;

		//hitArea->position = Vector2(position.x, position.y);
		hitArea->position.y = position.y;

		double distanceBetweenPosAndMax = maxPosition.y - position.y;
		/*if (minMaxDifference == 0)
			percentAt = 0;
		else*/
		percentAt = (minMaxDifference - distanceBetweenPosAndMax)
			/ (minMaxDifference);

		/*wostringstream ws;
		ws << "percentAt: " << percentAt;
		ws << " distanceBetweenPosAndMax: " << distanceBetweenPosAndMax;
		ws << " position.y: " << position.y;
		ws << " maxPosition.y: " << maxPosition.y << "\n";
		OutputDebugString(ws.str().c_str());*/

	} else if (isHover)
		tint = hoverColor;
	else
		tint = normalColor;
}

void Scrubber::scroll(double increment) {

	percentAt += increment;
	if (percentAt < 0)
		percentAt = 0;
	else if (percentAt > 1)
		percentAt = 1;

	position.y = (minMaxDifference * percentAt) + minPosition.y;
	hitArea->position.y = position.y;

	//wostringstream ws;
	//ws << "percentAt: " << percentAt;
	//ws << " increment: " << increment << "\n";
	//OutputDebugString(ws.str().c_str());
}


bool Scrubber::pressed() {
	/*if (isPressed) {
		isPressed = false;
		return true;
	}*/
	return isPressed;
}
/** **** Scrubber END **** **/