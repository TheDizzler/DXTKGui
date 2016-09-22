#include "Panel.h"

#include "../Engine/GameEngine.h"
Panel::Panel(GraphicsAsset* pixelAsset, ScrollBar* scrllbr) {
	//bgSprite.reset(new RectangleSprite(pixelAsset));
	hitArea.reset(new HitArea(Vector2::Zero, Vector2::Zero));
	verticalScrollBar.reset(scrllbr);
}


Panel::~Panel() {
}


void Panel::setTexture(unique_ptr<GraphicsAsset> gfx) {
	gfxAsset.release();
	gfxAsset = move(gfx);
}

void Panel::update(double deltaTime, MouseController* mouse) {

	if (viewRect.top < gfxAsset->getHeight())
		verticalScrollBar->update(deltaTime, mouse);

}

void Panel::draw(SpriteBatch* batch) {

	//bgSprite->draw(batch);
	if (gfxAsset != NULL) {
		batch->Draw(gfxAsset->getTexture().Get(), position, &viewRect, Color(1, 1, 1), rotation,
			origin, scale, SpriteEffects_None, layerDepth);

		if (viewRect.top < gfxAsset->getHeight() || alwaysDisplayScrollBar) {
			verticalScrollBar->draw(batch);
		}
	}
}

void Panel::setDimensions(const Vector2& pos, const Vector2& size) {


	viewRect.left = 0;
	viewRect.top = 0 /*- 25*/;
	viewRect.right = size.x;
	viewRect.bottom = /*-25 + */size.y;

	hitArea->size = size;
	setPosition(pos);
	Vector2 vertScrollBarPos =
		Vector2(position.x + size.x, position.y);
	verticalScrollBar->setPosition(vertScrollBarPos);
	verticalScrollBar->setBarHeight(size.y);
}


void Panel::moveBy(const Vector2 & moveVector) {
	GUIControl::moveBy(moveVector);
	verticalScrollBar->moveBy(moveVector);
	//bgSprite->moveBy(moveVector);
}

const Vector2& Panel::getPosition() const {
	return position;
}

const int Panel::getWidth() const {
	//return bgSprite->getWidth();
	return hitArea->size.x;
}

const int Panel::getHeight() const {
	//return bgSprite->getHeight();
	return hitArea->size.y;
}

bool Panel::clicked() {
	return isClicked;
}

bool Panel::pressed() {
	return isPressed;
}

bool Panel::hovering() {
	return isHover;
}

void Panel::setScrollBar(ScrollBarDesc& scrollBarDesc) {
	verticalScrollBar.reset(guiFactory->createScrollBar(
		Vector2(position.x + getWidth(), position.y),
		getHeight(), scrollBarDesc));
	verticalScrollBar->setScrollBar(1, getHeight(), 1);
}

void Panel::alwaysShowScrollBar(bool alwaysShow) {
	alwaysDisplayScrollBar = alwaysShow;
}


void Panel::setFont(const pugi::char_t* font) {
}

void Panel::setText(wstring text) {
}

XMVECTOR XM_CALLCONV Panel::measureString() const {
	return Vector2::Zero;
}
