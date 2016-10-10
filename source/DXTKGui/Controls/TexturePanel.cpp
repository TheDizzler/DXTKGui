#include "TexturePanel.h"

//#include "../../Engine/GameEngine.h"
TexturePanel::TexturePanel(GraphicsAsset* pixelAsset, ScrollBar* scrllbr) {

	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	verticalScrollBar.reset(scrllbr);
}


TexturePanel::~TexturePanel() {
}


void TexturePanel::setTexture(GraphicsAsset* gfx) {

	gfxAsset.release();
	gfxAsset.reset(gfx);
	texture = gfxAsset->getTexture();
	//origin = Vector2(gfxAsset->getWidth() / 2, gfxAsset->getHeight() / 2);
	//position.x += origin.x/2;
	//position.y += origin.y/2;

	if (gfxAsset->getHeight() > getHeight()) {
		verticalScrollBar->setScrollBar(
			gfxAsset->getHeight() / scrollSpeed, 1, getHeight() / scrollSpeed);
		showScrollBar = true;
	} else
		showScrollBar = false;
}

void TexturePanel::update(double deltaTime) {

	if (showScrollBar || alwaysDisplayScrollBar) {
		verticalScrollBar->update(deltaTime);

		if (hitArea->contains(mouse->getPosition())) {
			int mouseWheelDelta = mouse->scrollWheelValue();
			if (mouseWheelDelta != 0)
				verticalScrollBar->scrollByIncrement(-mouseWheelDelta);
		}

		// adjust viewRect according to scrollBar percent
		double movePercent = verticalScrollBar->percentScroll * gfxAsset->getHeight();
		viewRect.top = movePercent;
		viewRect.bottom = movePercent + getHeight();
	}
}

void TexturePanel::draw(SpriteBatch* batch) {

	if (gfxAsset != NULL) {
		batch->Draw(texture.Get(), position, &viewRect,
			Color(1, 1, 1, 1), rotation, origin, scale, SpriteEffects_None);

		if (showScrollBar || alwaysDisplayScrollBar) {
			verticalScrollBar->draw(batch);
		}
	}
}


void TexturePanel::setDimensions(const Vector2& pos, const Vector2& size) {

	viewRect.left = 0;
	viewRect.top = 0;
	viewRect.right = size.x;
	viewRect.bottom = size.y;

	hitArea->size = size;
	setPosition(pos);
	Vector2 vertScrollBarPos = Vector2(position.x + size.x, position.y);
	verticalScrollBar->setPosition(vertScrollBarPos);
	verticalScrollBar->setBarHeight(size.y);
}

void TexturePanel::setTexturePosition(const Vector2& texPos) {

	Vector2 diff = texPos - position;
	viewRect.right -= diff.x;
	viewRect.bottom -= diff.y;

	setPosition(texPos);
}

void TexturePanel::setScale(const Vector2& newScale) {
	GUIControl::setScale(newScale);
	verticalScrollBar->setScale(newScale);
}


void TexturePanel::moveBy(const Vector2 & moveVector) {
	GUIControl::moveBy(moveVector);
	verticalScrollBar->moveBy(moveVector);
}

const Vector2& TexturePanel::getPosition() const {
	return position;
}

const int TexturePanel::getWidth() const {
	return hitArea->size.x;
}

const int TexturePanel::getHeight() const {
	return hitArea->size.y;
}

const Vector2& TexturePanel::getScrollBarSize() const {
	return verticalScrollBar->getSize();
}

bool TexturePanel::clicked() {
	return isClicked;
}

bool TexturePanel::pressed() {
	return isPressed;
}

bool TexturePanel::hovering() {
	return isHover;
}

#include "GUIFactory.h"
void TexturePanel::setScrollBar(ScrollBarDesc& scrollBarDesc) {

	verticalScrollBar.reset(guiFactory->createScrollBar(
		Vector2(position.x + getWidth(), position.y),
		getHeight(), scrollBarDesc));
	int height = getHeight();
	if (gfxAsset != NULL)
		height = gfxAsset->getHeight();

	verticalScrollBar->setScrollBar(height / scrollSpeed, 1, viewRect.bottom / scrollSpeed);
}

void TexturePanel::alwaysShowScrollBar(bool alwaysShow) {
	alwaysDisplayScrollBar = alwaysShow;
}


void TexturePanel::setFont(const pugi::char_t* font) {
}

void TexturePanel::setText(wstring text) {
}

const Vector2& XM_CALLCONV TexturePanel::measureString() const {
	return Vector2::Zero;
}
