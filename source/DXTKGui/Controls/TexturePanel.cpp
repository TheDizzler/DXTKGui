#include "TexturePanel.h"

#include "../GUIFactory.h"
TexturePanel::TexturePanel(GUIFactory* factory, shared_ptr<MouseController> mouseController,
	ScrollBar* scrllbr) : GUIControl(factory, mouseController) {

	hitArea = make_unique<HitArea>(Vector2::Zero, Vector2::Zero);
	if (scrllbr == NULL)
		neverShowScrollBar = true;
	else
		verticalScrollBar.reset(scrllbr);
}

#include "../StringHelper.h"
TexturePanel::~TexturePanel() {

}


void TexturePanel::setTexture(unique_ptr<GraphicsAsset> gfx) {

	gfxAsset = move(gfx);
	texture = gfxAsset->getTexture();
	viewRect.left = 0;
	viewRect.top = 0;
	viewRect.right = gfxAsset->getWidth();
	viewRect.bottom = gfxAsset->getHeight();

	if (neverShowScrollBar)
		return;

	if (gfxAsset->getHeight() > getHeight()) {
		verticalScrollBar->setScrollBar(
			gfxAsset->getHeight() / scrollSpeed, 1, getHeight() / scrollSpeed);
		showScrollBar = true;
	} else
		showScrollBar = false;
}

bool TexturePanel::update(double deltaTime) {

	bool refreshed = false;

	if (!neverShowScrollBar && (showScrollBar || alwaysDisplayScrollBar)) {
		refreshed = verticalScrollBar->update(deltaTime);

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

	return refreshed;
}

void TexturePanel::draw(SpriteBatch* batch) {

	if (gfxAsset != NULL) {
		batch->Draw(texture.Get(), position, &viewRect,
			Color(1, 1, 1, 1), rotation, origin, scale, SpriteEffects_None, layerDepth);

		if (!neverShowScrollBar && (showScrollBar || alwaysDisplayScrollBar)) {
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
	if (neverShowScrollBar)
		return;
	Vector2 vertScrollBarPos = Vector2(position.x + size.x, position.y);
	verticalScrollBar->setPosition(vertScrollBarPos);
	verticalScrollBar->setBarHeight(size.y);
}

void TexturePanel::setTexturePosition(const Vector2& texPos) {

	Vector2 diff = texPos - position;
	viewRect.right += diff.x;
	viewRect.bottom += diff.y;

	setPosition(texPos);
}


void TexturePanel::setLayerDepth(const float newDepth, bool frontToBack) {

	layerDepth = newDepth;
	float nudge = .00000001;
	if (!frontToBack)
		nudge *= -1;

	if (neverShowScrollBar)
		return;
	verticalScrollBar->setLayerDepth(layerDepth + nudge, frontToBack);
}


void TexturePanel::setScale(const Vector2& newScale) {
	GUIControl::setScale(newScale);

	if (neverShowScrollBar)
		return;
	verticalScrollBar->setScale(newScale);
}


void TexturePanel::moveBy(const Vector2& moveVector) {

	setPosition(position + moveVector);

	if (neverShowScrollBar)
		return;
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
	if (neverShowScrollBar)
		return Vector2::Zero;
	return verticalScrollBar->getSize();
}

bool TexturePanel::scrollBarVisible() const {
	return showScrollBar || alwaysDisplayScrollBar;
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


void TexturePanel::setScrollBar(ScrollBarDesc& scrollBarDesc) {

	neverShowScrollBar = false;

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
